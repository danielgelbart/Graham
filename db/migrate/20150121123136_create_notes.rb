class CreateNotes < ActiveRecord::Migration
  def change
    create_table :notes do |t|
      t.integer :stock_id
      t.integer :year, limit:2, default:2014
      t.integer :pertains_to, limit:1, default:1
      t.string :note
      t.timestamps
    end
  end
end
