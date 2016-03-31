# == Schema Information
#
# Table name: share_classes
#
#  id         :integer(4)      not null, primary key
#  stock_id   :integer(4)
#  ticker     :string(8)      # value of '-' == NOT pubicly traded
#  sclass     :string(3)
#  votes      :integer(4)
#  nshares    :string(255)
#  float_date :date
#  mul_factor :integer(3)      default(1)
#  note       :string(255)
#  created_at :datetime        not null
#  updated_at :datetime        not null
#

# NOTE: If a share class has the ticker '-' then it is NOT publicly traded

# NOTE: mul_factor - If different for different publicly tradded classes,
#       market cap must be calculated using two seperate prices

# NOTE: 'nshares' indicates most recent share count (from date 'float_date')
# Historic values are shares at an annual basses in Ep table
class ShareClass < ActiveRecord::Base
  belongs_to :stock

  validates :stock, presence: true
  validates :ticker, presence: true, :uniqueness => {:scope => :stock_id}
  validates :sclass, :uniqueness => {:scope => :stock_id}
end
