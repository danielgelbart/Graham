class CreateSplits < ActiveRecord::Migration
  def self.up
    create_table :splits do |t|
      t.integer :stock_id
      t.integer :old
      t.integer :new
      t.decimal :split_mul_factor, :precision => 10, :scale => 2
      t.date :date
      t.boolean :stock_adjusted_for_split, :default => false

      t.timestamps
    end
  end

  def self.down
    drop_table :splits
  end
end
