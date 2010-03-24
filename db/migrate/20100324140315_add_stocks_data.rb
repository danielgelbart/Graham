class AddStocksData < ActiveRecord::Migration
  def self.up
    add_column :stocks, :market_cap, :integer
    add_column :stocks, :ttm_div, :decimal, :precision => 10, :scale => 3
    add_column :stocks, :yield, :decimal, :precision => 6, :scale => 3
  end

  def self.down
    remove_column :stocks, :market_cap
    remove_column :stocks, :ttm_div
    remove_column :stocks, :yield
  end
end
