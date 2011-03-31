class AddSalesToBs < ActiveRecord::Migration
  def self.up
      add_column :balance_sheets, :total_sales, :string
  end

  def self.down
      remove_column :balance_sheets, :total_sales
  end
end
