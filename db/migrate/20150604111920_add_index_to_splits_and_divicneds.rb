class AddIndexToSplitsAndDivicneds < ActiveRecord::Migration
  def change
    add_index :splits, :stock_id
    add_index :splits, :date
    add_index :dividends, :stock_id
    add_index :dividends, :ex_date
    add_index :dividends, :pay_date
  end
end
