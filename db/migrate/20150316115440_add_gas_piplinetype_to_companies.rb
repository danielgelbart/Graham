class AddGasPiplinetypeToCompanies < ActiveRecord::Migration
  def up
    change_column :stocks, :company_type, :enum, :limit => [:COMPANY, :ROYALTY_TRUST, :REIT, :ASSET_MNGMT, :FINANCE, :PARTNERSHIP, :PIPELINE, :HOLDING, :INDUSTRY, :TECH, :PHARMA, :RETAIL ], :default => :COMPANY
  end

  def down
    change_column :stocks, :company_type, :enum, :limit => [:COMPANY, :ROYALTY_TRUST, :REIT, :ASSET_MNGMT, :FINANCE, :HOLDING, :INDUSTRY, :TECH, :PHARMA, :RETAIL ], :default => :COMPANY
  end
end
