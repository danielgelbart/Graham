class Portfolio < ActiveRecord::Base
  belongs_to :user
  has_many :transactions
  has_many :owned_stocks

  validates_presence_of :user


  def get_owned_stock(stock)
    owned_stocks.detect{ |os| os.stock == stock }
  end

end

# == Schema Information
#
# Table name: portfolios
#
#  id         :integer(4)      not null, primary key
#  user_id    :integer(4)
#  name       :string(255)
#  created_at :datetime
#  updated_at :datetime
#

