class ShareClass < ActiveRecord::Base
  belongs_to :stock

  validates :stock, presence: true
  validates :ticker, presence: true, uniqueness: true


end
