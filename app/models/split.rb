class Split < ActiveRecord::Base
  belongs_to :stock

  validates :stock, presence: true
  validates :base, presence: true
  validates :into, presence: true


end
