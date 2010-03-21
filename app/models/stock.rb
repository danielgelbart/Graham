# == Schema Information
# Schema version: 20100317013419
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
#  revenue                :integer(4)
#

class Stock < ActiveRecord::Base
  has_many :splits
  has_many :owned_stocks
  has_many :dividends
  has_many :eps

  validates_presence_of :ticker
  validates_uniqueness_of :ticker

  include DataScraper


  # 50 million dollars in 1972 adjusted for present day inflation
  # perhaps should also take in to account the growth of the market size itself?
  MIN_SIZE = 250000000

#/ Valuation methods ---------------------------------------------------------

  def big_enough?
    # should be:
    #  assets || sales >= MIN_SIZE
    revenue  || 0 >= MIN_SIZE
  end

  def bargain?
    price * 1.5 <= book_value_per_share
  end

  # This is from page 62 of "Inteligent investor":
  # "An industrial company's finances are not conservative unless the common stock (at book value) represents at least half of the total capitalization, including all bank debt."
  # currently understood to mean that book value is at least half of price
  def conservativly_financed?
    book_value_per_share * 2 > price
  end

  def continous_dividend_record?(years = 20)
    current_year = Date.today.year

    dg = dividends.group_by{ |d| d.date.year }
    dg.reject{|dg| dg.size == current_yeat }.each do |year, divs|
      divs.size == dividends_per_year
    end
    update_dividends
    Date.today - max(365 / dividends_per_year, 120 ).days > newest_dividend.date
  end

  def cheap?
   # If the company has negative earnings we do not want to buy it so return false
  # If so, price_limit will return a negative value
    price < price_limit
  end

  # Upper price limit should be no more than:
  # 25 times average earnings over past 7 years, and
  # 20 times arerage earnings over past 1 year.
  def price_limit
    min( historic_eps(7)*25, ttm_eps*20 )
  end

  def good_defensive_stock?
    big_enough? && conservativly_financed? && countinous_dividend_record?
  end

  def good_defensive_buy?
    good_defensive_stock? && cheap?
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

 #/ End / Valuation methods --------------------------------------------------

 #/ Data retrenal methods ----------------------------------------------------

  # How long is this object instance alive for
  def price
    @price ||= get_stock_price
  end

  def newest_dividend
    dividends.sort_by{ |d| d.date }.last
  end

  def update_dividends
    if newest_dividend.date < Date.today - (365/dividends_per_year).days
      # get (newest?) data
    end
  end

  # Will update:
  # - ttm_eps
  # - book value/ share

  # This sould not be from a rake task.. Refactor this into the DataScraper class
  def update_current_data
    require 'nokogiri'
    require 'open-uri'
    url = "http://moneycentral.msn.com/investor/invsub/results/hilite.asp?Symbol=US%3a#{ticker}"

    begin
      doc = Nokogiri::HTML(open(url))
    rescue OpenURI::HTTPError => e
    end

    if doc && doc.xpath('//tr')
      tr = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Earnings/Share" }

      if tr
        ttm_eps = tr.xpath('./td').last.text.to_f

        book_value = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Book Value/Share" }.xpath('./td').last.text.to_f
      end
    end

    if !book_value #Try yahoo for retriving data
      url = "http://finance.yahoo.com/q/ks?s=#{ticker}"
      begin
        doc = Nokogiri::HTML(open(url))
      rescue OpenURI::HTTPError => e
      else
        if doc && doc.xpath('//tr')
          tr = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Diluted EPS (ttm):" }
          if tr
            ttm_eps = tr.xpath('./td').last.text.to_f

            book_value = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Book Value Per Share (mrq):" }.xpath('./td').last.text.to_f
          end
        end
      end
    end

    #Get sales data
    url = "http://finance.yahoo.com/q/ks?s=#{ticker}"
    begin
      doc = Nokogiri::HTML(open(url))
    rescue OpenURI::HTTPError => e
    else
      tr = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Revenue (ttm):" }
      if tr
        revenue = tr.xpath('./td').last.text
        revenue = case revenue.last
                  when "B"
                    revenue.chop.to_i * 1000000000
                  when "M"
                    revenue.chop.to_i * 1000000
                  else
                    revenue.to_i
                  end
      end
    end


    update_attributes!(:ttm_eps => ttm_eps,
                       :book_value_per_share => book_value,
                       :finantial_data_updated => Date.today,
                       :revenue => revenue)
  end

#/ End /Data retrenal methods ------------------------------------------------

  # This method should be mixed in as module, or from some other external source
  def min(a,b)
    a < b ? a : b
  end

  def max(a,b)
    a > b ? a : b
  end

  def to_param
    ticker
  end

end
