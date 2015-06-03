class RemoveMarketCapFromStocks < ActiveRecord::Migration
  def up
    remove_column :stocks, :market_cap
      end

  def down
    add_column :stocks, :market_cap, :string
  end
end
