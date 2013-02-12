class AddMarkToStocks < ActiveRecord::Migration
  def self.up
    add_column :stocks, :mark, :string
  end

  def self.down
    remove_column :stocks, :mark
  end
end
