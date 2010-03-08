# == Schema Information
# Schema version: 20100304214922
#
# Table name: portfolios
#
#  id         :integer         not null, primary key
#  user_id    :integer
#  name       :string(255)
#  created_at :datetime
#  updated_at :datetime
#

class Portfolio < ActiveRecord::Base
  belongs_to :user
  has_many :transactions
  has_many :owned_stocks



end
