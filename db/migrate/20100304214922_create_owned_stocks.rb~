class CreateOwnedStocks < ActiveRecord::Migration
  def self.up
    create_table :owned_stocks do |t|
      t.integer :portfolio_id
      t.integer :stock_id
      t.integer :shares

      t.timestamps
    end
  end

  def self.down
    drop_table :owned_stocks
  end
end
