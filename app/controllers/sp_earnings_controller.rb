class SpEarningsController < ApplicationController
  # GET /sp_earnings
  # GET /sp_earnings.json
  def index
    @sp_earnings = SpEarning.all

    respond_to do |format|
      format.html # index.html.erb
      format.json { render json: @sp_earnings }
    end
  end

  # GET /sp_earnings/1
  # GET /sp_earnings/1.json
  def show
    @sp_earning = SpEarning.find(params[:id])

    respond_to do |format|
      format.html # show.html.erb
      format.json { render json: @sp_earning }
    end
  end

  # GET /sp_earnings/new
  # GET /sp_earnings/new.json
  def new
    @sp_earning = SpEarning.new

    respond_to do |format|
      format.html # new.html.erb
      format.json { render json: @sp_earning }
    end
  end

  # GET /sp_earnings/1/edit
  def edit
    @sp_earning = SpEarning.find(params[:id])
  end

  # POST /sp_earnings
  # POST /sp_earnings.json
  def create
    @sp_earning = SpEarning.new(params[:sp_earning])

    respond_to do |format|
      if @sp_earning.save
        format.html { redirect_to @sp_earning, notice: 'Sp earning was successfully created.' }
        format.json { render json: @sp_earning, status: :created, location: @sp_earning }
      else
        format.html { render action: "new" }
        format.json { render json: @sp_earning.errors, status: :unprocessable_entity }
      end
    end
  end

  # PUT /sp_earnings/1
  # PUT /sp_earnings/1.json
  def update
    @sp_earning = SpEarning.find(params[:id])

    respond_to do |format|
      if @sp_earning.update_attributes(params[:sp_earning])
        format.html { redirect_to @sp_earning, notice: 'Sp earning was successfully updated.' }
        format.json { head :no_content }
      else
        format.html { render action: "edit" }
        format.json { render json: @sp_earning.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /sp_earnings/1
  # DELETE /sp_earnings/1.json
  def destroy
    @sp_earning = SpEarning.find(params[:id])
    @sp_earning.destroy

    respond_to do |format|
      format.html { redirect_to sp_earnings_url }
      format.json { head :no_content }
    end
  end
end
