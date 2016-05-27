class SpEarning < ActiveRecord::Base

  validates :calc_date, presence: true
  validates :list_file, presence: true
  validates :num_included, presence: true
  validates :total_market_cap, presence: true, format: { with: /\d*/,
    message: "only digits allowed" }
  validates :public_market_cap, presence: true, format: { with: /\d*/,
    message: "only digits allowed" }
  validates :total_earnings, presence: true, format: { with: /\d*/,
    message: "only digits allowed" }
  validates :market_pe, presence: true
  validates :index_price, presence: true
  validates :inferred_divisor, presence: true
  validates :divisor_earnings, presence: true
  validates :divisor_pe, presence: true


  def self.calc_earnings(list, save_to_db = false)

    #makret cap and earnings for entire stock list, in Billions
    total_market_cap = 0
    public_market_cap = 0
    total_earnings = 0

    comp_data = []


    failed_tickers = []

    output_file = File.open(Rails.root.join('log','sp_evaluation_out.txt'),"w")

    # search for latest list instead...

    CSV.foreach(Rails.root.join('sp500',get_latest_list)) do |row|
      ticker = row[0].strip
      ticker = "BRK.A" if ticker == "BRK-B"
      ticker = "BF.B" if ticker == "BF-B"

      #next if ticker.to_s == "GGP" # SCE site is missing the filings!!!
      if ticker.to_s == "CSRA" # NO annual data yet
        output_file.puts"Not enough data yet to include #{ticker}"
        @failed_tickers << ticker
        next
      end

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

      if stock.has_multiple_share_classes?
        next if @exclude_list.include?(stock.ticker)
        @exclude_list << stock.ticker
      end

      stock.update_price if(stock.updated_at < 1.days.ago)

      # Get more recent data if needed!
      stock.update_earnings if !stock.earnings_up_to_date?

      price = stock.price

      ep = stock.ttm_earnings_record

      #first filing for HPE is annual for 2015
      ep = stock.annual_eps_newest_first.first if stock.ticker == "HPE"

      if ep.nil?
        output_file.puts"Could not get latest earnings record for #{ticker}"
        @failed_tickers << ticker
        next
      end

      @comp

      spd = Spdata.new(stock.ticker,
                       price,
                       stock.ttm_eps,
                       ep.net_income,
                       stock.shares_float)
      @pub_market_cap += stock.public_market_cap
      @total_market_cap += spd.market_cap
      @total_earnings += spd.ttm_earnings.to_i

      @comp_data << spd
    end
    output_file.close


    #create DB entry to save calculations

    #create DB entry each time???
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

    @spe.save if (params[:save] == "true")


    @comp_data.sort_by! { |s| -s.market_cap }

    # get stock objects in list

    # update price and earnings if needed

    # calculate earnings/price/pe

    # save results

  end



  def self.get_index_price
    s = Stock.new
    s.get_price_from_google("INDEXSP%3A.INX")
  end

end
