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

    output_file = File.open(Rails.root.join('log','sp_evaluation_out.txt'),"w")
    CSV.foreach(Rails.root.join('utility_scripts','sp500_list_2016-01-18.txt')) do |row|
      ticker = row[0]

      # still havn't diceded what to do with multiple share classes:
      next if ticker.to_s == "CMCSA"
      next if ticker.to_s == "COP"
      next if ticker.to_s == "GGP" # SCE site is missing the filings!!!

      stock = Stock.find_by_ticker(ticker)

      if stock.nil?
        output_file.puts"Could not get stock object for ticker#{ticker}"
        next
      end
      price = stock.price

      ep = stock.ttm_earnings_record

      if ep.nil?
        output_file.puts"Could not get latest earnings record for #{ticker}"
        next
      end

      spd = Spdata.new(stock.ticker, price, stock.ttm_eps,
                       ep.net_income, stock.shares_float)

      @total_market_cap += spd.market_cap
      @total_earnings += spd.ttm_earnings.to_i

      @comp_data << spd
    end
    output_file.close
    @comp_data.sort_by! { |s| -s.market_cap }
  end


end
