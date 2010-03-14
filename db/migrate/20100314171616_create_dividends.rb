class CreateDividends < ActiveRecord::Migration
  def self.up
    create_table :dividends do |t|
      t.integer :stock_id
      t.date :date
      t.decimal :amount, :precision => 12, :scale => 8
      t.string :source

      t.timestamps
    end
  end

  def self.down
    drop_table :dividends
  end
end
