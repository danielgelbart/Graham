class AddAssetsSalesToStocks < ActiveRecord::Migration
  def self.up
    rename_column :stocks, :revenue, :sales
    add_column :stocks, :assets, :integer
  end

  def self.down
    rename_column :stocks, :sales, :revenue
    remove_column :stocks, :assets
  end
end
