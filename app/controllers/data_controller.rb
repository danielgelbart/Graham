class DataController < ApplicationController

require 'csv'
  def sppe
    #read file

    # @divisor = 8718720000 # Latest divisor 8718.72from 3/31/2016 - retrieved may 2016

    #makret cap and earnings for entire stock list, in Billions
    @total_market_cap = 0
    @pub_market_cap = 0
    @total_earnings = 0
    @index = 0 #get from google?
    @comp_data = []
    #lines = lines.first(10) #test first

    @multiticker_list = []
    @failed_tickers = []

    output_file = File.open(Rails.root.join('log','sp_evaluation_out.txt'),"w")

    # search for latest list instead...

    CSV.foreach(Rails.root.join('sp500',get_latest_list)) do |row|
      ticker = row[0].strip
      ticker = "BRK.B" if ticker == "BRK-B"
      ticker = "BF.B" if ticker == "BF-B"

      #next if ticker.to_s == "GGP" # SCE site is missing the filings!!!
=begin      if ticker.to_s == "CSRA" # NO annual data yet
        output_file.puts"Not enough data yet to include #{ticker}"
        @failed_tickers << ticker
        next
     end
=end

      # Acquired - no longer listed:
      next if ticker.to_s == "PCP" # Acquired - no longer listed
      next if ticker.to_s == "BRCM" # Acquired - no longer listed

      stock = Stock.get_from_ticker(ticker)

      if (stock.nil? && ticker)
        ticker = ticker.gsub("-","")
        stock = Stock.joins(:share_classes).where( share_classes: {ticker: ticker}).first
      end

      if stock.nil?
        output_file.puts"Could not get stock object for ticker#{ticker}"
        @failed_tickers << ticker
        next
      end

      if stock.has_multiple_public_classes?
        sc = Stock.get_stock_class_by_ticker(ticker)
      end

      if params[:price] == "Y"
        stock.update_price
      else
        stock.update_price if(stock.updated_at < 1.days.ago)
      end

      # Get more recent data if needed!
      if params[:update] == "Y"
        if ticker != "PRGO"
          stock.update_financials if !stock.earnings_up_to_date?
        end
      end

      price = sc.nil? ? stock.price : stock.get_price_from_google("",sc.ticker)

      ep = stock.ttm_earnings_record

      #first filing for HPE is annual for 2015
      ep = stock.annual_eps_newest_first.first if stock.ticker == "HPE"

      if ep.nil?
        output_file.puts"Could not get latest earnings record for #{ticker}"
        @failed_tickers << ticker
        next
      end

      #new
      net_income = ep.net_income.to_i
      ttm_eps = stock.ttm_eps

      #make sure share_of_float does as you think
      if !sc.nil?
        # net_income = net_income * sc.share_of_float
        # ttm_eps = (net_income * sc.share_of_float)/ sc.nshares.to_i
      end

      num_shares = sc.nil? ? stock.shares_float : sc.nshares.to_i
      ticker = sc.ticker if !sc.nil?
      spd = Spdata.new(ticker,
                       price,
                       ttm_eps,
                       net_income,
                       num_shares)

      @pub_market_cap += sc.nil? ? stock.public_market_cap : sc.market_cap

      if !@multiticker_list.include?(stock.ticker)
        @total_market_cap += stock.market_cap.to_i
        @total_earnings += ep.net_income.to_i
      end

      @multiticker_list << stock.ticker if !sc.nil?

      @comp_data << spd
    end
    output_file.close

    #create DB entry to save calculations
    @index_price = SpEarning.get_index_price
    @inferred_divisor = (@pub_market_cap.to_f / @index_price)
    @divisor_earnings = (@total_earnings.to_f / @inferred_divisor)
    @spe = SpEarning.new( calc_date: Date.today,
                          list_file: get_latest_list,
                          num_included: @comp_data.size,
                          excluded_list: ar_to_s(@failed_tickers),
                          total_market_cap: @total_market_cap.to_i.to_s,
                          public_market_cap: @pub_market_cap.to_i.to_s,
                          total_earnings: @total_earnings.to_i.to_s,
                          market_pe: (@total_market_cap / @total_earnings.to_f),
                          index_price: @index_price,
                          inferred_divisor: @inferred_divisor,
                          divisor_earnings: @divisor_earnings,
                          divisor_pe: (@index_price / @divisor_earnings),
                          notes: "created from data_controller#sppe")

    @spe.save if (params[:save] == "Y")

    @comp_data.sort_by! { |s| -s.market_cap }

  end

  def get_latest_list( date = "")
    file_regex = /^sp500_list_\d\d.*\.txt$/
    date_regex = /(\d\d\d\d-\d\d-\d\d)/
    sp_lists = Dir.entries("sp500/").select{ |f| f =~ file_regex }

    return "" if sp_lists.empty?

    latest = "1980-01-01"
    sp_lists.each do |fs|
      fs_date = fs[date_regex,1].to_date
      l_date = latest[date_regex,1].to_date
      latest = fs if  l_date < fs_date
    end

    return latest
  end

  def ar_to_s(arr)
    str = arr.shift.to_s
    arr.each do |ar|
      str += " , " + ar.to_s
    end
  end


end
