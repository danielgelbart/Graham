class CreateSplits < ActiveRecord::Migration
  def change
    create_table :splits do |t|
      t.integer :stock_id
      t.date :date
      t.integer :base
      t.integer :into

      t.timestamps
    end
  end
end
