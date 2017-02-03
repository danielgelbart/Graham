class AddDilutedDataToEps < ActiveRecord::Migration
  def change
    change_column_default(:eps, :shares_diluted, false)
    add_column :eps, :eps_diluted, :boolean, :default => false
  end
end
