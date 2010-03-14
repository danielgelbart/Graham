# == Schema Information
# Schema version: 20100314171616
#
# Table name: dividends
#
#  id         :integer(4)      not null, primary key
#  stock_id   :integer(4)
#  date       :date
#  amount     :decimal(10, 2)
#  source     :string(255)
#  created_at :datetime
#  updated_at :datetime
#

class Dividend < ActiveRecord::Base


end
