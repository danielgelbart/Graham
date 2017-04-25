class AddIndexEarningsToSpEarnings < ActiveRecord::Migration
  def change
    add_column :sp_earnings, :index_market_cap, :string
  end
end
