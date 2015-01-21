class CreateNotes < ActiveRecord::Migration
  def change
    create_table :notes do |t|
      t.integer :stock_id
      t.integer :year, limit:2, default:0
      t.integer :pertains_to, limit:1, default:0
      t.string :note

      t.timestamps
    end
  end
end
