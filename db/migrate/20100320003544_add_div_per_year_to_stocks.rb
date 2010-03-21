class AddDivPerYearToStocks < ActiveRecord::Migration
  def self.up
    add_column :stocks, :dividends_per_year, :integer, :default => 4
  end

  def self.down
    remove_column :stocks, :dividends_per_year
  end
end
