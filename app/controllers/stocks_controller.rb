class StocksController < ApplicationController

  def index
    @stocks = Stock.all
  end

  def bargains
    @bargains = Stock.all.select{ |s| s.bargain? }
  end

  def show
    # This acceps bot id and ticker to find the stock
    # wrap this in a helper or before filter
    if (params[:id]).to_i > 0
      @stock = Stock.find_by_ticker(params[:id])
    else
      @stock = Stock.find_by_ticker(params[:id])
    end
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
    @stock = Stock.find(params[:id])
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
    @stock = Stock.find(params[:id])

    respond_to do |format|
      if @stock.update_attributes(params[:stock])
        flash[:notice] = 'Stock was successfully updated.'
        format.html { redirect_to(@stock) }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @stock.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /stocks/1
  # DELETE /stocks/1.xml
  def destroy
    @stock = Stock.find(params[:id])
    @stock.destroy

    respond_to do |format|
      format.html { redirect_to(stocks_url) }
      format.xml  { head :ok }
    end
  end
end
