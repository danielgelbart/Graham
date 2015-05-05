class RemoveBVfromFromStocks < ActiveRecord::Migration
  def up
    remove_column :stocks, :book_value_per_share
  end

  def down
    add_column :stocks, :book_value_per_share, :decimal, :precision => 12, :scale => 6
  end
end
