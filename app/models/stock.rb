# == Schema Information
#
# Table name: stocks
#
#  id                 :integer(4)      not null, primary key
#  name               :string(255)
#  ticker             :string(255)
#  created_at         :datetime
#  updated_at         :datetime
#  dividends_per_year :integer(4)      default(4)
#  latest_price       :decimal(12, 6)
#  ttm_div            :decimal(10, 3)
#  yield              :decimal(6, 3)
#  listed             :boolean(1)      default(TRUE)
#  has_currant_ratio  :boolean(1)      default(TRUE)
#  mark               :string(255)
#  cik                :integer(4)
#  fiscal_year_end    :string(255)     default("")
#  company_type       :enum([:COMPANY, default(:COMPANY)
#  country            :string(255)
#  fy_same_as_ed      :boolean(1)      default(TRUE)
#

#enum EnumStockCOMPANY_TYPE { :COMPANY, :ROYALTY_TRUST, :REIT, :ASSET_MNGMT, :FINANCE, :PARTNERSHIP, :PIPELINE, :FOREIGN, :HOLDING, :INDUSTRY, :TECH, :PHARMA, :RETAIL ], :default => :COMPANY

class Stock < ActiveRecord::Base

  has_many :dividends
  has_many :notes
  has_many :eps, :dependent => :destroy
  has_many :numshares, :dependent => :destroy
  has_many :balance_sheets, :dependent => :destroy
  has_many :splits

  accepts_nested_attributes_for :balance_sheets, :allow_destroy => true
  accepts_nested_attributes_for :eps, :allow_destroy => true

  validates_presence_of :ticker
  validates_uniqueness_of :ticker

  include DataScraper

  # for handling enums ------------------------------------------
  def self.enum_columns
    # can be made dynamic wite columns_hash.each{ |c| c.sql_type}
    [:company_type]
  end

  def self.enum_field?(col_name)
    return enum_columns.include?(col_name)
  end

  def self.enum_options(col_name)
    enum_col = Stock.columns_hash[col_name.to_s]
    ar = enum_col.limit
    ar = enum_string_to_values_array(enum_col.sql_type) if ar.class == Fixnum
    i = 0
    Hash[ar.map {|v| [v, i=i+1]}]
  end

  def self.enum_string_to_values_array(str)
    str_arr = str[6..-3].split(/','/)
  end
# end of enum methods code
#--------------------------------------------


  #include MinMAx # adds min and max methods

  # 50 million dollars in 1972 adjusted for present day inflation
  # perhaps should also take in to account the growth of the market size itself?
  MIN_SALES = 500000000 # 500mil
  MIN_BV = 2500000000 # 2.5B

  # defining this hear, since for some reason it was not working if set from data_scraper
  YEAR = Time.new.year

#/ Valuation methods ---------------------------------------------------------

  # 1) Adequate size
  def big_enough?
    lbs = latest_balance_sheet
    lep = annual_eps_newest_first.first

    if !lbs.nil? && !lep.nil? && !lbs.book_val.nil? && !lep.revenue.nil?
      ret=( lep.revenue.to_i >= MIN_SALES || lbs.book_val >=MIN_BV)
    end

    if ret.nil? && !lbs.nil? && !lbs.assets_t.nil? && !lbs.liabilities_t.nil?
      ret = lbs.equity >= MIN_BV
    end

    ret.nil? ? false : ret
  end

  # 2) Finantialy strong
  # current assets > current liabilaties * 2   # For industrial
  # long term debt < current assets
  # debt < 2 * stock equity (at book value) # for public utilities

  def financialy_strong?
    bs = latest_balance_sheet
    if bs
      cr = bs.current_ratio
      alr = cr >= 2 if !cr.nil?
       alr = bs.assets_t >= bs.liabilities_t * 2 if alr.nil? && !bs.assets_t.nil? && !bs.liabilities_t.nil?
      dr = bs.debt < bs.assets_c if !bs.debt.nil? && !bs.assets_c.nil?
      dr = bs.debt < bs.assets_t if dr.nil? && !bs.debt.nil? && !bs.assets_t.nil?
    end
    (alr && dr) || false
  end

  # 3) Earnings stability
  # No loses in past 10 years
  def no_earnings_deficit?
    epss = annual_eps_newest_first.first(10)

    earning_deficit = epss.select{ |e| e.eps < 0 }
    earning_deficit.empty?
  end

  # 4) Continuos dividend record
  # This is pushed as the most important
  # Shold have uninterupted dividends over past 20 years

  # Beware! does not include current year
  def continous_dividend_record?(years = 20)
    current_year = YEAR
    dg = dividends.group_by{ |d| d.date.year }

    (current_year - years..current_year - 1).each do |year|
      return false if dg[year].nil? || (!dg[year].nil? && dg[year].size < 2)
    end
    true
  end

  # 5) Earnings growth
  # This needs to be adjusted for stock splits/new offers/float ?
  # If every succeeding 3 year period is better than the previous
  def eps_growth?
    epss = annual_eps_oldest_first

    eps_avg(epss.first(max(epss.size / 2,3))) * 1.3 <= eps_avg(epss.last(3))
  end

  # 6) Moderate price (to earnings)
  # Upper price limit should be no more than:
  # 25 times average earnings over past 7 years, and
  # 20 times arerage earnings over past 1 year.

  # second criteria from page 182
  def price_limit
    # earning records do not go back far enough to compute price limit
    return 0 if historic_eps(7).nil? || ttm_eps.nil?

# || historic_eps(7).to_i == 0

    lim = min( historic_eps(7).to_f*25, ttm_eps*20 )
    min( historic_eps(3) * 15, lim ) # second criteria from page 182
  end

  # 7) Moderate price (to book)
  def asset_to_price_ratio?
    price / ttm_eps * ( price / book_value_per_share ) <= 22.5
  end

  def price_to_limit_ratio
    return 0.0 if price_limit.nil? or price_limit == 0
    (price.to_f / price_limit.to_f)
  end

  def price_to_book_ratio
    return 0.0 if book_value_per_share.nil? or book_value_per_share == 0
    price.to_f / book_value_per_share.to_f
  end


  # / End, Defensive buy breackdown---------------------------------------

  def cheap?
    return false if price.nil? or price_limit.nil?
    price < price_limit
  end

  # Set a price at wich a stock is overvalued to the point of concidering to sell it
  def valuation_limit
    return 1000000 if historic_eps(10).nil? || ttm_eps.nil?
    return historic_eps(3) * 28 if historic_eps(10).to_i == 0 # historic eps returns string!
    lim = max( historic_eps(10) * 26, ttm_eps*40 )
    lim = max( historic_eps(3) * 28, lim ) # second criteria from page 182
    return 1000000 if lim.nil?
    lim
  end

  def overpriced?
    return false if price.nil?
    price > valuation_limit
  end

  def good_defensive_stock?
    big_enough? && financialy_strong? && no_earnings_deficit? && eps_growth? && continous_dividend_record? # dividents data does ont work?
  end

  def good_defensive_buy?
    good_defensive_stock? && cheap? #&& asset_to_price_ratio?
  end

  def bargain?
    (price * 1.5) < book_value_per_share #Not working in rails 3: 'price' is somehow morphed to a nokogiri::element  of some sort
    false
  end

  # This is from page 62 of "Inteligent investor":
  # "An industrial company's finances are not conservative unless the common stock (at book value) represents at least half of the total capitalization, including all bank debt."
  # currently understood to mean that book value is at least half of price
  def conservativly_financed?
    book_value_per_share * 2 >= price
  end


  #/ Data retreval methods ----------------------------------------------------

  def price
    @price ||= latest_price
  end

  def update_price
    p = get_stock_price
    if !p.nil?
      update_attributes!(:latest_price => p)
      @price = p
    end
    price
  end

  def update_price_if(num_days)
    update_price if updated_at < num_days.days.ago
    price
  end

  def newest_dividend
    dividends.sort_by{ |d| d.date }.last
  end

  def oldest_dividend
    dividends.sort_by{ |d| d.date }.first
  end

  def pays_dividends
    !dividends.empty? && newest_dividend.date > Date.today - 6.months
  end

  def update_dividends
    if newest_dividend.date < Date.today - (365/dividends_per_year).days
      # get (newest?) data
    end
  end

  #/ End /Data retrenal methods ------------------------------------------------

  def inflation_ratio_for(year)

    #please UPDATE!
    #Last updated: May 2015
    # uses inflation from every year, so that I don't need to update
    # Calculated as change in CPI, from jan 1 to jan 1, durring the given year,
    # i.e data for 2013 is for change ending jan 1 2014.
    # using "ALL Urban Consumers LESS Food & Energy (i.e. CORE inflation)
    # Source: https://research.stlouisfed.org/fred2/release?rid=10
    # Saved to file localy
    ir = {
      2000 => 1.02581,
      2001 => 1.02735,
      2002 => 1.01917,
      2003 => 1.01149,
      2004 => 1.02169,
      2005 => 1.02174,
      2006 => 1.02573,
      2007 => 1.02439,
      2008 => 1.01763,
      2009 => 1.01816,
      2010 => 1.00804,
      2011 => 1.0223,
      2012 => 1.01893,
      2013 => 1.01717,
      2014 => 1.01606
      }


    # Now muliply the earnings from a given year, by all the years AFTER it
    mul = 1
    last_year = YEAR - 1
    return mul if year == last_year

    ((year + 1)..(last_year)).each do |i|
      mul *= ir[i]
    end

    mul
  end


  # eps methods -------------------------------------------------------------

# returns all previous eps per adjusted for inflation
  def adjust_for_inflation(eps)
     eps.map{ |e| inflation_ratio_for(e.year)*e.net_income.to_i }
  end

  def recent_annual_earnings(years)
    annual_eps_newest_first.first(years)
  end

  # Returns nil if earnings record does not exist going 'years' back
  def historic_eps(years)

    if annual_eps_newest_first.size < years
      return "Do not have #{years} of earnings for #{ticker}"
    end

    current_year = YEAR
    # Create copy array with only last number of years
    recent = annual_eps_newest_first.first(years)

    recent = adjust_for_inflation(recent)
    # Calculate the avarage
    avrege_earnings = recent.inject(0.0){|sum, e| sum + e } / years
    avrege_earnings / shares_float.to_f
  end

  def ten_year_eps
    ds = annual_eps_newest_first.size
    return  price / historic_eps(10) if ds >= 10 && !price.nil?
    0.0
  end

  def return_on_equity(income,equity)
    ( income.to_i / equity.to_f ) * 100
  end

  def historic_roe(years)
    roe = 0
    recent_earnings = annual_eps_newest_first.first(years)
    num_years = 0
    recent_earnings.each do |e|
      bs = balance_sheets.where(year: e.year).first
      next if bs.nil?
      roe += return_on_equity(e.net_income.to_i,bs.book_val)
      num_years += 1
    end
    roe / num_years
  end

# Stock dilution
  def dilution(num)
    return 0 if annual_eps_newest_first[num-1].nil?
    latest = annual_eps_newest_first[0]
    first = annual_eps_newest_first[num-1]
    return 0 if first.shares.to_i == 0 || latest.shares.to_i == 0
    dil_rate =  latest.shares.to_i.to_f / first.shares.to_i.to_f
  end

  # Gets most recent balance sheet, regardles if updated
  def latest_balance_sheet
    balance_sheets.sort{ |b,y| b.year <=> y.year }.last
  end

  def book_value
    latest_balance_sheet.book_val
  end

  def shares_float
    newest_earnings_record.shares.to_i
  end

  def market_cap
    shares_float * price
  end

  # Gets most recent earnings, regardles if updated
  def newest_earnings_record
    eps.sort_by{ |e| [e.year,e.quarter] }.last
  end

  # this is for any eps, quarterly OR annual
  def most_recent_eps
    eps.sort_by{ |e| [e.year,e.quarter] }.last
  end

  # this is for annual
  def latest_eps
    annual_eps_newest_first.first
  end

  #OLDEST FIRST
  def annual_eps_oldest_first
    epss = eps.select{ |e| e.quarter == 0 }
    epss.sort!{ |a,b| a.year <=> b.year }
  end

  #NEWEST FIRST
  def annual_eps_newest_first
    annual_eps_oldest_first.sort!{ |a,b| b.year <=> a.year }
  end

  def ep_for_year(year)
    eps.select{ |e| (e.year == year) && (e.quarter == 0) }.first
  end

  def book_value_per_share
    book_value / latest_eps.shares.to_f
  end

  def ncav
    latest_balance_sheet.ncav
  end

  def ncav_ratio
    translate_to_int(market_cap) / ncav
  end

  def translate_to_int(str)
    if str.match(/\d+\.\d+\w/)
      res = case str.chop
            when "B", "Bil"
              str.chop.to_f * BILLION
            when "M", "Mil"
               str.chop.to_f * MILLION
            else
              str.chop.to_f * BILLION
            end
      return res
    end
  end

  def dividend_url
    "http://dividata.com/stock/#{ticker}/dividend"
  end

  def to_param
    ticker.gsub(/\./,"")
  end

  def ttm_earnings_record
    eps.select{ |e| e.quarter == 5}.first
  end

  def ttm_eps
    ttm_record = ttm_earnings_record
    ttm = ttm_record.eps if !ttm_record.nil?
    ttm ||= latest_eps.eps
  end

  def pe
    price / ttm_eps.to_f
  end

  def is_quarterly?
    quarter > 0
  end

  def be_arr_hash
    years = []
    year_to_check = annual_eps_newest_first.first.year
    while(true)
      if (!eps.where( year: year_to_check, quarter: 0).empty? &&
          !balance_sheets.where( year: year_to_check).empty?)
        years << year_to_check
        year_to_check -= 1
      else
        break
      end
    end

    be_arr = []
    years.each do |year|
      be_arr.insert(0,data_hash(year))
    end
    be_arr
  end

  def data_hash(year)
    dhash = {}
    e = eps.where( year: year, quarter: 0).first
    b = balance_sheets.where( year: year).first
    dhash[:year] = year
    dhash[:net_income] = e.net_income.to_i
    dhash[:revenue] = e.revenue.to_i
    dhash[:equity] = b.equity.to_i
    dhash[:total_debt] = b.debt.to_i
    dhash
  end


  # Math module

  def min(a,b)
    a < b ? a : b
  end

  def max(a,b)
    a > b ? a : b
  end

  def eps_avg(set)
    set.inject(0.0){|sum, e| sum + e.eps} / set.size
  end

  # String.to_i
  class String

    def translate_letter_to_number
      if str.match(/\d+\.?\d+\w/)
        res = case str.chop
              when "B"
                str.chop.to_f * BILLION
              when "M"
               str.chop.to_f * MILLION
              else
                str.chop.to_f * BILLION
              end
      end
      res
    end

    def clean_numeric_string
      gsub(/\$|,|\302|\240/,"").strip
    end

    def to_i
      #translate B and M to Bilion and Million (numaricly)
      clean_numeric_string
      translate_letter_to_number ||  s.super.to_i
    end

  end #string class



end


