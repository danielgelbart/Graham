class SplitsController < ApplicationController
  def new
    @split = Split.new
  end

  def create
    @split = Split.new(params[:split])

    if @split.save
      flash[:notice] = 'Split was successfully created.'
      redirect_to :action => "index"
    else
      render :action => "new"
    end
  end

  def index
    @splits = Split.all
  end

  def destroy
    @split = Split.find(params[:id])
    @split.destroy
    redirect_to(splits_url)
  end

end
