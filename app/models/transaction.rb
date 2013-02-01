class Transaction < ActiveRecord::Base
  belongs_to :stock
  belongs_to :portfolio
  after_create :add_owned_stock

  delegate :ticker, :to => :stock
  # validates_format_of :type, :with => /(BUY)|(SELL)/


  def buy?
    type == "BUY"
  end

  def sell?
    type == "SELL"
  end

  private

  def add_owned_stock
    owned_stock = Portfolio.find(1).get_owned_stock(stock)

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

# == Schema Information
#
# Table name: transactions
#
#  id           :integer(4)      not null, primary key
#  date         :date
#  comission    :decimal(10, 2)
#  stock_id     :integer(4)
#  shares       :integer(4)
#  type         :boolean(1)
#  price        :decimal(10, 2)
#  portfolio_id :integer(4)
#  created_at   :datetime
#  updated_at   :datetime
#

