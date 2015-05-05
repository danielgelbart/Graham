class RemoveSalsesFromBs < ActiveRecord::Migration
  def up
    remove_column :balance_sheets, :total_sales
  end

  def down
    add_column :balance_sheets, :total_sales, :string
  end
end
