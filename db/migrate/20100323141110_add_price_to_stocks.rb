class AddPriceToStocks < ActiveRecord::Migration
  def self.up
    add_column :stocks, :latest_price, :decimal, :precision => 12, :scale => 6
  end

  def self.down
    remove_column :stocks, :latest_price
  end
end
