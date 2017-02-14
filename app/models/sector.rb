# == Schema Information
#
# Table name: sectors
#
#  id   :integer(4)      not null, primary key
#  name :string(255)
#

class Sector < ActiveRecord::Base
  has_many :sub_sectors
  has_many :stocks, through: :sub_sectors
  validates_presence_of :name
  validates_uniqueness_of :name
end
