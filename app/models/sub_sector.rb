# == Schema Information
#
# Table name: sub_sectors
#
#  id         :integer(4)      not null, primary key
#  name       :string(255)
#  sector_id  :integer(4)
#  created_at :datetime        not null
#  updated_at :datetime        not null
#

class SubSector < ActiveRecord::Base
  belongs_to :sector
  has_many :stocks
  validates_presence_of :sector_id
  validates_presence_of :name
  validates_uniqueness_of :name, :scope => :sector_id
end
