class SpEarning < ActiveRecord::Base

  validates :calc_date, presence: true
  validates :list_file, presence: true
  validates :num_included, presence: true
  validates :total_market_cap, presence: true, format: { with: /\d*/,
    message: "only digits allowed" }
  validates :public_market_cap, presence: true, format: { with: /\d*/,
    message: "only digits allowed" }
  validates :total_earnings, presence: true, format: { with: /\d*/,
    message: "only digits allowed" }
  validates :market_pe, presence: true
  validates :index_price, presence: true
  validates :inferred_divisor, presence: true
  validates :divisor_earnings, presence: true
  validates :divisor_pe, presence: true



  def self.get_index_price
    s = Stock.new
    s.get_price_from_google("INDEXSP%3A.INX")
  end

end
