# == Schema Information
#
# Table name: dividends
#
#  id         :integer(4)      not null, primary key
#  stock_id   :integer(4)
#  date       :date
#  amount     :decimal(12, 8)
#  source     :string(255)
#  created_at :datetime
#  updated_at :datetime
#

class Dividend < ActiveRecord::Base
  belongs_to :stock

  validates :stock, presence: true
  validates :ex_date, uniqueness: { scope: :stock_id }
  validates :pay_date, uniqueness: { scope: :stock_id }
#  validates_uniqueness_of :date, :scope => :stock_id

  def date
    pay_date.nil? ? ex_date : pay_date
  end

end

