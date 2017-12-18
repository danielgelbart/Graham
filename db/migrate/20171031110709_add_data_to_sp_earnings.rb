class AddDataToSpEarnings < ActiveRecord::Migration
  def change
    add_column :sp_earnings, :median_pe, :integer
    add_column :sp_earnings, :losers, :integer
  end
end
