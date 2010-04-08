class CreateTransactions < ActiveRecord::Migration
  def self.up
    create_table :transactions do |t|
      t.date :date
      t.decimal :comission, :precision => 10, :scale => 2
      t.integer :stock_id
      t.integer :shares
      t.boolean :type
      t.decimal :price, :precision => 10, :scale => 2
      t.integer :portfolio_id
      t.timestamps
    end
  end

  def self.down
    drop_table :transactions
  end
end
