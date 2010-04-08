class CleanUpStock < ActiveRecord::Migration
  def self.up
    remove_column :stocks, :sales
    remove_column :stocks, :assets
    remove_column :stocks, :total_debt
  end

  def self.down
    add_column :stocks, :sales, :integer
    add_column :stocks, :assets, :integer
    add_column :stocks, :total_debt, :integer
  end
end
