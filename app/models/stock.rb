# == Schema Information
# Schema version: 20100401155417
#
# Table name: stocks
#
#  id                     :integer(4)      not null, primary key
#  name                   :string(255)
#  ticker                 :string(255)
#  created_at             :datetime
#  updated_at             :datetime
#  total_debt             :integer(4)
#  ttm_eps                :decimal(12, 6)
#  book_value_per_share   :decimal(12, 6)
#  finantial_data_updated :date
#  sales                  :integer(4)
#  dividends_per_year     :integer(4)      default(4)
#  latest_price           :decimal(12, 6)
#  assets                 :integer(4)
#  market_cap             :string(255)
#  ttm_div                :decimal(10, 3)
#  yield                  :decimal(6, 3)
#

class Stock < ActiveRecord::Base
  has_many :splits
  has_many :owned_stocks
  has_many :dividends
  has_many :eps, :dependent => :destroy
  has_many :balance_sheets

  validates_presence_of :ticker
  validates_uniqueness_of :ticker

  include DataScraper
  #include MinMAx # adds min and max methods

  # 50 million dollars in 1972 adjusted for present day inflation
  # perhaps should also take in to account the growth of the market size itself?
  MIN_SALES = 500000000
  MIN_ASSETS = 250000000

#/ Valuation methods ---------------------------------------------------------

  # 1) Adequate size
  def big_enough?
   # sales && assets && (sales >= MIN_SIZE || assets >= MIN_SIZE)
    sales && sales >= MIN_SALES
  end

  # 2) Finantialy strong
  # current assets > current liabilaties * 2   # For industrial
  # long term debt < current assets

  # debt < 2 * stock equity (at book value) # for public utilities

  def financialy_strong?
    bs = latest_balance_sheet
    if bs
      alr = bs.assets_c >= bs.liabilities_c * 2 if bs.assets_c && bs.liabilities_c
      alr = bs.assets_t >= bs.liabilities_t * 2 if alr.nil? && bs.assets_t && bs.liabilities_t
      dr = bs.debt < bs.assets_c if bs.debt && bs.assets_c
      dr = bs.debt < bs.assets_t if dr.nil? && bs.debt && bs.assets_t
    end
    (alr && dr) || false
  end



  # 3) Earnings stability
  # No loses in past 10 years
  def no_earnings_deficit?
    earning_deficit = eps.select{ |e| e.eps < 0 }
    earning_deficit.empty?
  end

  # 4) Continuos dividend record
  # This is pushed as the most important
  # Shold have uninterupted dividends over past 20 years

  # Beware! does not include current year
  def continous_dividend_record?(years = 20)
    current_year = Date.today.year
    dg = dividends.group_by{ |d| d.date.year }

    (current_year - years..current_year - 1).each do |year|
      return false if !(!dg[year].nil? && dg[year].size >= 2)
    end
    true
  end

  # 5) Earnings growth
  # This needs to be adjusted for stock splits/new offers/float ?
  def eps_growth?
    epss = eps.sort_by{ |e| e.year }
    eps_avg(epss.first(3)) * 1.3 <= eps_avg(epss.last(3))
  end

  # 6) Moderate price (to earnings)
  # Upper price limit should be no more than:
  # 25 times average earnings over past 7 years, and
  # 20 times arerage earnings over past 1 year.

  # second criteria from page 182
  def price_limit
    lim = min( historic_eps(7)*25, ttm_eps*20 )
    min( historic_eps(3) * 15, lim ) # second criteria from page 182
  end

  # 7) Moderate price (to book)
  def asset_to_price_ratio?
    price / ttm_eps * ( price / book_value_per_share ) <= 22.5
  end

  # / End, Defensive buy breackdown---------------------------------------

  def cheap?
    price < price_limit
  end

  def good_defensive_stock?
    big_enough? && financialy_strong? && no_earnings_deficit? && eps_growth? && continous_dividend_record?
  end

  def good_defensive_buy?
    good_defensive_stock? && cheap? && asset_to_price_ratio?
  end

  # How to give this method a dynamic name like historic_eps_7_years_back?
  def historic_eps(years)
    update_current_data if !eps_records_up_to_date?
    current_year = Date.today.year.to_i
    recent = eps.select{|e| e.year > current_year -1 - years }
    recent.inject(0.0){|sum, e| sum + e.eps} / years
  end

  def eps_records_up_to_date?
    year = Date.today.year.to_i
    10.times do
      year = year - 1
      return false if !eps.detect{ |e| e.year == year}
    end
    !ttm_eps.nil? && ttm_eps != 0
  end

  def bargain?
    price * 1.5 <= book_value_per_share
  end

  # This is from page 62 of "Inteligent investor":
  # "An industrial company's finances are not conservative unless the common stock (at book value) represents at least half of the total capitalization, including all bank debt."
  # currently understood to mean that book value is at least half of price
  def conservativly_financed?
    book_value_per_share * 1.5 >= price
  end

  #/ Data retreval methods ----------------------------------------------------

  def price
    @price ||= latest_price
  end

  def update_price
    p = get_stock_price
    if p
      update_attributes!(:latest_price => p)
      @price = p
    end
    price
  end

  def newest_dividend
    dividends.sort_by{ |d| d.date }.last
  end

  def oldest_dividend
    dividends.sort_by{ |d| d.date }.first
  end

  def update_dividends
    if newest_dividend.date < Date.today - (365/dividends_per_year).days
      # get (newest?) data
    end
  end

  def update_current_data
    ttm_eps = get_eps
    book_value = get_book_value
    sales = get_sales
    div = get_ttm_div

    if ttm_eps && book_value
      update_attributes!(:ttm_eps => ttm_eps,
                         :book_value_per_share => book_value,
                         :sales => sales,
                         :finantial_data_updated => Date.today)
    end

    def update_price_data
      update_price
      get_market_cap
    end
  end

  #/ End /Data retrenal methods ------------------------------------------------
  def latest_balance_sheet
    balance_sheets.detect{ |bs| bs.year == Date.today.year - 1}
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
            when "B"
              str.chop.to_f * BILLION
            when "M"
               str.chop.to_f * MILLION
            else
              str.chop.to_f * BILLION
            end
      return res
    end
  end

  def dividend_url
    "http://www.dividend.com/historical/stock.php?symbol=#{ticker}"
  end

  def to_param
    ticker
  end

  def min(a,b)
    a < b ? a : b
  end

  def max(a,b)
    a > b ? a : b
  end

  def eps_avg(set)
    set.inject(0.0){|sum, e| sum + e.eps} / set.size
  end
end
