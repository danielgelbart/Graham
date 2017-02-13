class SearchesController < ApplicationController
  helper_method :sort_column, :sort_direction

  # GET /searches
  # GET /searches.xml
  def index
    @searches = Search.all

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @searches }
    end
  end

  # GET /searches/1
  # GET /searches/1.xml
  def show
    @search = Search.find(params[:id])

    @stocks = @search.stocks( sort_column, sort_direction )

    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @search }
    end
  end

  # GET /searches/new
  # GET /searches/new.xml
  def new
    @search = Search.new

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @search }
    end
  end

  # GET /searches/1/edit
  def edit
    @search = Search.find(params[:id])
  end

  # POST /searches
  # POST /searches.xml
  def create
    @search = Search.new(params[:search])
    @params = params

    respond_to do |format|
      if @search.save
        format.html { redirect_to(@search, :notice => 'Search was successfully created.') }
        format.xml  { render :xml => @search, :status => :created, :location => @search }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @search.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /searches/1
  # PUT /searches/1.xml
  def update
    @search = Search.find(params[:id])

    respond_to do |format|
      if @search.update_attributes(params[:search])
        format.html { redirect_to(@search, :notice => 'Search was successfully updated.') }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @search.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /searches/1
  # DELETE /searches/1.xml
  def destroy
    @search = Search.find(params[:id])
    @search.destroy

    respond_to do |format|
      format.html { redirect_to(searches_url) }
      format.xml  { head :ok }
    end
  end

private
  def sortable_columns
    ["pe", "pe10", "Market Cap", "max_pe", "price_to_book", "price_to_limit"]
  end

  def sort_column
    @sort_col = params[:column]
    sortable_columns.include?(params[:column]) ? params[:column] : "Market Cap"
  end

  def sort_direction
    @sort_dirc = params[:direction]
    %w[asc desc].include?(params[:direction]) ? params[:direction] : "disc"
  end

end
