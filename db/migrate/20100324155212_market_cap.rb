class MarketCap < ActiveRecord::Migration
  def self.up
    change_column :stocks, :market_cap, :string
  end

  def self.down
    change_column :stocks, :market_cap, :integer
  end
end
