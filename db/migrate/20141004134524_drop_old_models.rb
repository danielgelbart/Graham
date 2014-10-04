class DropOldModels < ActiveRecord::Migration
  def up
    drop_table :owned_stocks
    drop_table :transactions
    drop_table :splits
    drop_table :portfolios

  end

  def down
  end
end
