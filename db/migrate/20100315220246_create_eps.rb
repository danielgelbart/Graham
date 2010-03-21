class CreateEps < ActiveRecord::Migration
  def self.up
    create_table :eps do |t|
      t.integer :year
      t.string :source
      t.decimal :eps, :precision => 12, :scale => 6
      t.integer :stock_id

      t.timestamps
    end
  end

  def self.down
    drop_table :eps
  end
end
