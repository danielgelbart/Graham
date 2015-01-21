class AddFiscalYearEndDateToStocks < ActiveRecord::Migration
  def change
    remove_column :stocks, :fiscal_year_end
    add_column :stocks, :fiscal_year_end, :string, :default => "";
  end
end
