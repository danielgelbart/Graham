class StocksController < ApplicationController

  def index
    @stocks = Stock.search(params[:search])
  end

  def bargains
    @bargains = Stock.all.select{ |s| s.bargain? }
  end

  def ncavs
    @stocks = Stock.all.select{ |s| s.latest_balance_sheet && s.ncav && s.translate_to_int(s.market_cap) != nil }
    @stocks = @stocks.select{ |s| s.ncav_ratio <= 1.1 }
  end

  def defensive_buys
    @stocks = Stock.all.select{ |s| s.good_defensive_buy? }
    @stocks = @stocks.sort_by{ |s| s.yield * -1}
  end

  def defensive_stocks
    @stocks = Stock.all.select{ |s| s.good_defensive_stock? }
    @stocks = @stocks.sort_by{ |s| s.price.to_f / s.price_limit.to_f }
  end

  def cheap_stocks
    @stocks = Stock.all.select{ |s| s.cheap? }
    @stocks = @stocks.select{ |s| s.price_limit > 1 }
    @stocks = @stocks.sort_by{ |s| s.price / s.price_limit }
  end

  def dear_stocks
    @stocks = Stock.all.select{ |s| s.overpriced? }
    @stocks = @stocks.select{ |s| s.valuation_limit < 1000000 }
    @stocks = @stocks.sort_by{ |s| s.ten_year_eps * -1 }
  end

  def pots
    @stocks = Stock.all.select{ |s| s.no_earnings_deficit? }
    @stocks = @stocks.select{ |s| s.dilution < 1.11 } # less than 10% dilution
    @stocks = @stocks.select{ |s| s.continous_dividend_record? } # has dividends
    @stocks = @stocks.sort_by{ |s| s.ten_year_eps }
  end

  def cheap_profitables
    @stocks = Stock.all.select{ |s| s.no_earnings_deficit? }
    @stocks = @stocks.select{ |s| s.dilution < 1.11 } # less than 10% dilution
    @stocks = @stocks.select{ |s| s.latest_balance_sheet.equity > 0 } # Positive book value
    @stocks = @stocks.select{ |s| s.financialy_strong?} # Ratios at least 2 to 1
    @stocks = @stocks.select{ |s| s.ten_year_eps < 20 && (s.price < s.ttm_eps*10 || s.price < s.historic_eps(3)*8) } # Cheap: defined as: less than 20 ten year PE and 8 3 year PE OR 10 current year PE
    @stocks = @stocks.sort_by{ |s| s.historic_eps(3) }
  end

  def show
    # This acceps both id and ticker to find the stock
    # wrap this in a helper or before filter
    id = params["id"]

    if (id.to_i > 0)
      @stock = Stock.find_by_ticker(id)
    else
      @stock = Stock.find_by_ticker(id)
      @stock = Stock.all.select{ |s| s.to_param == id }.first if @stock.nil?
    end

    @stock.update_price
    # realy all that is needed for constructing chart
    @earnings = @stock.annual_eps_oldest_first

    # 1) copy all numshare data into eps table in dev environment
    @numshare = @earnings.map{|s| s.shares.to_i }
    @income = @earnings.map{|s| s.net_income.to_i }
    @revenue = @earnings.map{|s| s.revenue.to_i }
    @notes = @stock.notes

  #=begin TTM EPS
    @ttm = @stock.ttm_earnings_record
    if !@ttm.nil?
      @income << @ttm.net_income.to_i
      @revenue << @ttm.revenue.to_i
      @numshare << @ttm.shares.to_i
    end
  #=end
  end

  # GET /stocks/new
  # GET /stocks/new.xml
  def new
    @stock = Stock.new

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @stock }
    end
  end

  # GET /stocks/1/edit
  def edit
    @stock = Stock.find_by_ticker(params[:id])
  end

  # POST /stocks
  # POST /stocks.xml
  def create
    @stock = Stock.new(params[:stock])

    respond_to do |format|
      if @stock.save
        flash[:notice] = 'Stock was successfully created.'
        format.html { redirect_to(@stock) }
        format.xml  { render :xml => @stock, :status => :created, :location => @stock }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @stock.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /stocks/1
  # PUT /stocks/1.xml
  def update
     @stock = Stock.find_by_ticker(params[:id])

    if @stock.update_attributes(params[:stock])
      flash[:notice] = 'Stock was successfully updated.'
    end
    redirect_to(@stock)
  end

  # DELETE /stocks/1
  # DELETE /stocks/1.xml
  def destroy
    @stock = Stock.find_by_ticker(params[:id])
    @stock.destroy

    respond_to do |format|
      format.html { redirect_to(stocks_url) }
      format.xml  { head :ok }
    end
  end
end
