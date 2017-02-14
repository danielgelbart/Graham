class AddFieldsToStocks < ActiveRecord::Migration

  def change
    add_column :stocks, :sub_sector_id, :integer
    add_column :stocks, :ipo_year, :integer
    add_index :stocks, :ticker
    add_index :stocks, :sub_sector_id
  end
end
