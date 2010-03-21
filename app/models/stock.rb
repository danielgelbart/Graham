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
  include Math # adds min and max methods

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
    puts ticker
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
    # update_dividends
    Date.today - max(365 / dividends_per_year, 120 ).days > newest_dividend.date
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

  #/ Data retrenal methods ----------------------------------------------------

  def price
    @price ||= get_stock_price
  end

  def update_price
    @price = get_stock_price
  end

  def newest_dividend
    dividends.sort_by{ |d| d.date }.last
  end

  def update_dividends
    if newest_dividend.date < Date.today - (365/dividends_per_year).days
      # get (newest?) data
    end
  end

  def update_current_data
    ttm_eps = get_eps
    book_value = get_book_value

    if ttm_eps && book_value
      update_attributes!(:ttm_eps => ttm_eps,
                         :book_value_per_share => book_value,
                         :finantial_data_updated => Date.today)
    end
  end

  #/ End /Data retrenal methods ------------------------------------------------

  def to_param
    ticker
  end

end
