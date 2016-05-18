# == Schema Information
#
# Table name: splits
#
#  id         :integer(4)      not null, primary key
#  stock_id   :integer(4)
#  date       :date
#  base       :integer(4)
#  into       :integer(4)
#  created_at :datetime        not null
#  updated_at :datetime        not null
#

class Split < ActiveRecord::Base
  belongs_to :stock

  validates :stock, presence: true
  validates :base, presence: true
  validates :into, presence: true
  validates :date, presence: true
  validates_uniqueness_of :date, :scope => [:stock_id]

end
