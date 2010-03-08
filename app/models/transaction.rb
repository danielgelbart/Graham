# == Schema Information
# Schema version: 20100304214922
#
# Table name: transactions
#
#  id         :integer         not null, primary key
#  date       :date
#  comission  :decimal(10, 2)
#  stock_id   :integer
#  shares     :integer
#  price      :decimal(10, 2)
#  created_at :datetime
#  updated_at :datetime
#

class Transaction < ActiveRecord::Base
  belongs_to :stock
  belongs_to :portfolio
  after_create :adjust_owned_stock


  def adjust_owned_stock
    owned_stock = portfolio.get_owned_stock(stock)

    if type == "BUY"
      if owned_stock
        owned_stock.update_attributes(owned_stock.shares + self.shares)
      else
        portfolio.owned_stock.build(:stock_id => stock.id,
                                    :portfolio_id => portfolio.id,
                                    :shares => shares)
      end
    else
      raise "This stock is not owned, so cannot be sold" if owned_stock.nil?
      raise "Trying to sell more shares than are owned" if self.shares > owned_stock.shares
      if self.shares == owned_stock.shares
        owned_stock.delete
      else
        owned_stock.update_attributes(owned_stock.shares - self.shares)
      end
    end
  end

end
