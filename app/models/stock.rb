# == Schema Information
# Schema version: 20100324155212
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

  validates_presence_of :ticker
  validates_uniqueness_of :ticker

  include DataScraper
  #include MinMAx # adds min and max methods

  # 50 million dollars in 1972 adjusted for present day inflation
  # perhaps should also take in to account the growth of the market size itself?
  MIN_SIZE = 250000000

#/ Valuation methods ---------------------------------------------------------

  def big_enough?
   # sales && assets && (sales >= MIN_SIZE || assets >= MIN_SIZE)
    sales && sales >= MIN_SIZE
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

  # Not perfect but a quick and dirty little method
  def long_divs?
    !dividends.empty? && oldest_dividend.date < Date.today - 20.years
  end

  # Beware! does not include current year
  def continous_dividend_record?(years = 20)
    current_year = Date.today.year
    dg = dividends.group_by{ |d| d.date.year }

    (current_year - years..current_year - 1).each do |year|
      return false if !(!dg[year].nil? && dg[year].size >= 2)
    end
    true
  end

  def cheap?
    price < price_limit
  end

  # Upper price limit should be no more than:
  # 25 times average earnings over past 7 years, and
  # 20 times arerage earnings over past 1 year.
  def price_limit
    min( historic_eps(7)*25, ttm_eps*20 )
  end

  def good_defensive_stock?
    big_enough? && conservativly_financed? && continous_dividend_record? #eps
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

  #/ Data retrenal methods ----------------------------------------------------

  def price
    @price ||= latest_price
  end

  def update_price
    p = get_stock_price
    if p
      update_attribute!(:latest_price => p)
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


    puts ticker

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
end
