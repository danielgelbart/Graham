class AddTypeToStocks < ActiveRecord::Migration
  def change
    add_column :stocks, :company_type, :enum, :limit => [:COMPANY, :ROYALTY_TRUST, :REIT, :ASSET_MNGMT, :HOLDING, :INDUSTRY, :TECH, :PHARMA, :RETAIL ], :default => :COMPANY
  end
end
