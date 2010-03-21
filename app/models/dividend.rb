# == Schema Information
# Schema version: 20100317013419
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

  validates_uniqueness_of :date, :scope => :stock_id


end
