class EpsController < ApplicationController
  # GET /eps
  # GET /eps.xml
  def index
    @eps = Ep.all

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @eps }
    end
  end

  # GET /eps/1
  # GET /eps/1.xml
  def show
    @ep = Ep.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @ep }
    end
  end

  # GET /eps/new
  # GET /eps/new.xml
  def new
    @ep = Ep.new

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @ep }
    end
  end

  # GET /eps/1/edit
  def edit
    @ep = Ep.find(params[:id])
  end

  # POST /eps
  # POST /eps.xml
  def create
    @ep = Ep.new(params[:ep])

    respond_to do |format|
      if @ep.save
        flash[:notice] = 'Ep was successfully created.'
        format.html { redirect_to(@ep) }
        format.xml  { render :xml => @ep, :status => :created, :location => @ep }
      else
        format.html { render :action => "new" }
        format.xml  { render :xml => @ep.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /eps/1
  # PUT /eps/1.xml
  def update
    @ep = Ep.find(params[:id])

    respond_to do |format|
      if @ep.update_attributes(params[:ep])
        flash[:notice] = 'Ep was successfully updated.'
        format.html { redirect_to(@ep) }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @ep.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /eps/1
  # DELETE /eps/1.xml
  def destroy
    @ep = Ep.find(params[:id])
    @ep.destroy

    respond_to do |format|
      format.html { redirect_to(eps_url) }
      format.xml  { head :ok }
    end
  end
end
