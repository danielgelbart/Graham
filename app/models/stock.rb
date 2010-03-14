# == Schema Information
# Schema version: 20100314171616
#
# Table name: stocks
#
#  id         :integer(4)      not null, primary key
#  name       :string(255)
#  ticker     :string(255)
#  created_at :datetime
#  updated_at :datetime
#

class Stock < ActiveRecord::Base
  has_many :splits
  has_many :owned_stocks
  has_many :dividends


  # 50 million dollars in 1972 adjusted for present day inflation
  # perhaps should also take in to account the growth of the market size itself?
  MIN_SIZE = 250000000


  # Neaded data for calculations:
  def get_data
  # currant

    # currant accounting
    assets
    sales
    book_value # does this include debt ?
    total_capitalization # = num shares * price ?
    debt

    # currant/easy
    price
    pe_ratio

    # historic
    dividend_record # for past 20 years
    pe_ratio_for_past_7_years

  end



  def big_enough?
    assets || sales >= MIN_SIZE
  end

  def conservativly_financed?
    (book_value + debt) * 2 > total_capitalization
  end

  def countinous_dividend_record?
    dividends #what to test for
  end

  # Upper price limit should be no more than:
  # 25 times average earnings over past 7 years, and
  # 20 times arerage earnings over past 1 year.
  def cheap?
    (price < pe_ratio_for_past_7_years * 25) && (price < pe_ratio * 20)
  end

  def good_defensive_stock?
    big_enough? && conservativly_financed? && countinous_dividend_record?
  end

  def good_defensive_buy?
    good_defensive_stock? && cheap?
  end

end
