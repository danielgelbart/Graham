class AddFieldsToSearches < ActiveRecord::Migration
  def self.up
    add_column :searches, :good_balance, :boolean
    add_column :searches, :book, :integer
    add_column :searches, :current_div, :boolean
    add_column :searches, :earning_growth, :boolean
    add_column :searches, :defensive_price, :boolean
    add_column :searches, :big_enough, :boolean
    add_column :searches, :market_cap, :integer
  end

  def self.down
    remove_column :searches, :good_balance
    remove_column :searches, :book
    remove_column :searches, :current_div
    remove_column :searches, :earning_growth
    remove_column :searches, :defensive_price
    remove_column :searches, :big_enough
    remove_column :searches, :market_cap
  end
end
