class AddCikToStocks < ActiveRecord::Migration
  def self.up
    add_column :stocks, :cik, :integer
  end

  def self.down
    remove_column :stocks, :cik
  end
end
