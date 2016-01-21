class Spdata

  def initinalize(ticker, price, ttm_eps,
                  earnings, shares)
    @ticker = ticker
    @price = price
    @ttm_eps = ttm_eps
    @ttm_earnings = earnings
    @num_shares = shares
  end

  def market_cap
    return 1 if @price.nil?
    return 0 if @num_shares.nil?
    @price * @num_shares
  end

  def ttm_earnings
    return 0.0001 if @ttm_earnings.nil?
  end

end


class DataController < ApplicationController
  require 'csv'

  def sppe
    #read file

    #makret cap and earnings for entire stock list, in Billions
    @total_market_cap = 0
    @total_earnings = 0
    @index = 0 #get from google?
    @comp_data = []
    #lines = lines.first(10) #test first

    CSV.foreach(Rails.root.join('utility_scripts','sp500_list_2016-01-18.txt')) do |row|
      ticker = row[0]

      stock = Stock.find_by_ticker(ticker)

      next if stock.nil?

      price = stock.price

      ep = stock.ttm_earnings_record

      next if ep.nil?

      spd = Spdata.new(stock.ticker, price, stock.ttm_eps,
                       ep.net_income, stock.shares_float)

      @total_market_cap += spd.market_cap
      @total_earnings += spd.ttm_earnings

      @comp_data << spd
    end

  end


end
