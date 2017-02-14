class CreateSubSectors < ActiveRecord::Migration
  def change
    create_table :sub_sectors do |t|
      t.string :name
      t.belongs_to :sector, index: true, foreign_key: true
      t.timestamps
    end

    add_index :sub_sectors, :name
  end
end
