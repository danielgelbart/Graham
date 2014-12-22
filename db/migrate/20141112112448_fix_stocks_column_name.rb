class FixStocksColumnName < ActiveRecord::Migration
  def up
    rename_column :stocks, :finantial_data_updated, :fiscal_year_end
  end

  def down
    rename_column :stocks, :fiscal_year_end, :finantial_data_updated
  end
end
