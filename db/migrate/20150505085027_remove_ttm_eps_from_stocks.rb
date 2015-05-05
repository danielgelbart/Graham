class RemoveTtmEpsFromStocks < ActiveRecord::Migration
  def up
    remove_column :stocks, :ttm_eps
  end

  def down
    add_column :stocks, :ttm_eps, :decimal, :precision => 12, :scale => 6
  end
end
