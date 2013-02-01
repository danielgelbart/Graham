class Ep < ActiveRecord::Base

  validates_uniqueness_of :year, :scope => :stock_id
  validates_presence_of :stock_id
 
  def margin
    if net_income && revenue
     net_income.to_f / revenue.to_f
    else
     0
    end
  end
end

# == Schema Information
#
# Table name: eps
#
#  id         :integer(4)      not null, primary key
#  year       :integer(4)
#  source     :string(255)
#  eps        :decimal(12, 6)
#  stock_id   :integer(4)
#  created_at :datetime
#  updated_at :datetime
#  revenue    :string(255)
#  net_income :string(255)
#

