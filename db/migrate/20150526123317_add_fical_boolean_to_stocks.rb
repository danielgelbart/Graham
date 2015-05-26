class AddFicalBooleanToStocks < ActiveRecord::Migration
  def change
    add_column :stocks, :fy_same_as_ed, :boolean, :default => true
  end
end
