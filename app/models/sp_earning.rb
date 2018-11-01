# == Schema Information
#
# Table name: sp_earnings
#
#  id                :integer(4)      not null, primary key
#  calc_date         :date
#  list_file         :string(255)
#  num_included      :integer(4)
#  excluded_list     :string(255)
#  total_market_cap  :string(255)
#  public_market_cap :string(255)
#  total_earnings    :string(255)
#  market_pe         :decimal(15, 2)
#  index_price       :float
#  inferred_divisor  :integer(10)
#  divisor_earnings  :decimal(15, 2)
#  divisor_pe        :decimal(15, 2)
#  notes             :string(255)
#  created_at        :datetime        not null
#  updated_at        :datetime        not null
#  index_market_cap  :string(255)
#

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


  def excluded
    # TODO: return a ruby array instead of a string
    excluded_list
  end

  def self.calc_earnings(list, save_to_db = false)
    # This method is not used in Graham application, so might be outdated
  end

  def self.get_index_price
    s = Stock.new
    s.get_price("","%5EGSPC")
  end

  def losing_percent
    (losers.to_f / num_included.to_f) * 100
  end

end

