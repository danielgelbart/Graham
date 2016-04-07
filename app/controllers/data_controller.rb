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
      ticker = "BRK.A" if ticker == "BRK-B"

      next if ticker.to_s == "GGP" # SCE site is missing the filings!!!
      next if ticker.to_s == "CSRA" # NO annual data yet

      stock = Stock.get_from_ticker(ticker)

      if (stock.nil? && ticker)
        ticker = ticker.gsub("-","")
        stock = Stock.joins(:share_classes).where( share_classes: {ticker: ticker}).first
      end

      if stock.nil?
        output_file.puts"Could not get stock object for ticker#{ticker}"
        next
      end
      price = stock.price

      ep = stock.ttm_earnings_record

      ep = stock.annual_eps_newest_first.first if stock.ticker == "BXLT" || "CPGX"

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
