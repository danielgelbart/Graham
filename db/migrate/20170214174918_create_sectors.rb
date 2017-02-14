class CreateSectors < ActiveRecord::Migration
  def change
    create_table :sectors do |t|
      t.string :name
    end

    add_index :sectors, :name
  end
end
