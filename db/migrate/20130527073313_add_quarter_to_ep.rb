class AddQuarterToEp < ActiveRecord::Migration
  def self.up
    add_column :eps, :quarter, :integer, :default => 0
  end

  def self.down
    remove_column :eps, :quarter
  end
end
