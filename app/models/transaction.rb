# == Schema Information
# Schema version: 20100401155417
#
# Table name: transactions
#
#  id           :integer(4)      not null, primary key
#  date         :date
#  comission    :decimal(10, 2)
#  stock_id     :integer(4)
#  shares       :integer(4)
#  price        :decimal(10, 2)
#  created_at   :datetime
#  updated_at   :datetime
#  portfolio_id :integer(4)
#

class Transaction < ActiveRecord::Base
  belongs_to :stock
  belongs_to :portfolio
  after_create :add_owned_stock

  delegate :ticker, :to => :stock

  private

  def add_owned_stock
    owned_stock = portfolio.get_owned_stock(stock)

    if owned_stock
      shares = owned_stock.shares
      owned_stock.update_attributes!(:shares => shares + self.shares)
    else
      portfolio.owned_stocks << OwnedStock.create!(:stock_id => stock.id,
                                                   :portfolio_id => portfolio.id,
                                                   :shares => self.shares)
    end
  end

  def sell_owned_stock
    owned_stock = portfolio.get_owned_stock(stock)
    raise "This stock is not owned, so cannot be sold" if owned_stock.nil?
    raise "Trying to sell more shares than are owned" if self.shares > owned_stock.shares
    if self.shares == owned_stock.shares
      owned_stock.delete
    else
      owned_stock.update_attributes(owned_stock.shares - self.shares)
    end
  end



end
