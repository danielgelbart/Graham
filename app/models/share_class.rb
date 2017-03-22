# == Schema Information
#
# Table name: share_classes
#
#  id            :integer(4)      not null, primary key
#  stock_id      :integer(4)
#  ticker        :string(8)
#  sclass        :string(3)
#  votes         :integer(4)
#  nshares       :string(255)
#  float_date    :date
#  mul_factor    :decimal(20, 10) default(1.0)
#  note          :string(255)
#  created_at    :datetime        not null
#  updated_at    :datetime        not null
#  latest_price  :decimal(12, 6)  default(0.0)
#  primary_class :boolean(1)      default(FALSE)
#


# NOTE: If a share class has the ticker '-' then it is NOT publicly traded

# NOTE: mul_factor - Is the CONVERTION ratio of one share class to another. For this porpuse, there must be defined a main (class 'A') share class. This only relates to:
# BRK: 1 BRK.A = 1500 BRK.B shares

# NOTE: 'nshares' indicates most recent share count (from date 'float_date')
# Historic values are shares at an annual basses in Ep table
class ShareClass < ActiveRecord::Base

  belongs_to :stock

  validates :stock, presence: true
  validates :ticker, presence: true, :uniqueness => {:scope => :stock_id}
  validates :sclass, :uniqueness => {:scope => :stock_id}

  before_save :check_primary, if: :primary_class_changed?

  include IConnection

  def share_of_float
    nshares.to_f / stock.shares_float
  end

  def price
    update_price if updated_at < 1.days.ago
    latest_price
  end

  def market_cap
    if is_public_class?
      nshares.to_i * price
    else
      sc_price = stock.primary_class.nil? ? stock.price : stock.primary_class.price
      nshares.to_i * mul_factor * sc_price
    end
  end

  def update_price
    return if !is_public_class?
    puts "Going to update price for #{ticker}"
    p = get_price("",ticker)
    update_attributes!(:latest_price => p) if !p.nil?
  end

  def is_public_class?
    ticker[0] != '-'
  end

  private

  #NOTE this will not roll back the save
  def check_primary
    if (primary_class)
      if stock.share_classes.select{ |sc| sc.primary_class }.empty?
        mul_factor = 1
      else
        errors.add(:primary_class, "Another share class is already defined as 'primary'")
      end
    end
  end


end

