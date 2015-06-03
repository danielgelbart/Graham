class ChangeColumnMarketCapForSearches < ActiveRecord::Migration
  def up
    change_column :searches, :market_cap, :string
  end

  def down
    change_column :searches, :market_cap, :integer
  end
end
