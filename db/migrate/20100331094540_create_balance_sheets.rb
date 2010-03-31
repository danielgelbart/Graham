class CreateBalanceSheets < ActiveRecord::Migration
  def self.up
    create_table :balance_sheets do |t|
      t.integer :stock_id
      t.integer :year
      t.string :current_assets
      t.string :total_assets
      t.string :current_liabilities
      t.string :total_liabilities
      t.string :long_term_debt
      t.string :book_value

      t.timestamps
    end
  end

  def self.down
    drop_table :balance_sheets
  end
end
