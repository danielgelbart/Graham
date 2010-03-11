# == Schema Information
# Schema version: 20100309193254
#
# Table name: stocks
#
#  id         :integer         not null, primary key
#  name       :string(255)
#  ticker     :string(255)
#  created_at :datetime
#  updated_at :datetime
#

class Stock < ActiveRecord::Base
  has_many :splits
  has_many :owned_stocks
  
end
