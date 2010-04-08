class AddListedToStocks < ActiveRecord::Migration
  def self.up
    add_column :stocks, :listed, :boolean, :default => 1
  end

  def self.down
    remove_column :stocks, :listed
  end
end
