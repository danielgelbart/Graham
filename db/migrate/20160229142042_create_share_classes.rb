class CreateShareClasses < ActiveRecord::Migration
  def change
    create_table :share_classes do |t|
      t.references :stock
      t.string :ticker, limit:8
      t.string :sclass, limit:3
      t.integer :votes
      t.string :nshares
      t.date :float_date
      t.integer :mul_factor, limit:3, default:1
      t.string :note

      t.timestamps
    end
    add_index :share_classes, :stock_id
    add_index :share_classes, :ticker
    add_index :share_classes, :sclass
  end
end
