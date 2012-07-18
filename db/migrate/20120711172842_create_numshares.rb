class CreateNumshares < ActiveRecord::Migration
  def self.up
    create_table :numshares do |t|
      t.integer :stock_id
      t.integer :year
      t.string  :shares

      t.timestamps
    end
  end

  def self.down
    drop_table :numshares
  end
end
