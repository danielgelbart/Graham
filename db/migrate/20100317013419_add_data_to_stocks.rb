class AddDataToStocks < ActiveRecord::Migration
  def self.up
    add_column :stocks, :ttm_eps, :decimal, :precision => 12, :scale => 6
    add_column :stocks, :book_value_per_share, :decimal, :precision => 12, :scale => 6
    add_column :stocks, :finantial_data_updated, :date
    add_column :stocks, :revenue, :integer

  end

  def self.down
    remove_column :stocks, :ttm_eps
    remove_column :stocks, :book_value_per_share
    remove_column :stocks, :finantial_data_updated
    remove_column :stocks, :revenue
  end
end
