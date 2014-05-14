class CreateSearches < ActiveRecord::Migration
  def self.up
    create_table :searches do |t|
      t.boolean :no_losses
      t.boolean :divs
      t.float :dilution

      t.timestamps
    end
  end

  def self.down
    drop_table :searches
  end
end
