# == Schema Information
# Schema version: 20100317013419
#
# Table name: users
#
#  id         :integer(4)      not null, primary key
#  name       :string(255)
#  created_at :datetime
#  updated_at :datetime
#

class User < ActiveRecord::Base
  has_many :portfolios
end
