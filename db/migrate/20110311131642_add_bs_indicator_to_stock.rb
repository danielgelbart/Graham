class AddBsIndicatorToStock < ActiveRecord::Migration
  def self.up
     add_column :stocks, :has_currant_ratio, :boolean, :default => true
  end

  def self.down
     remove_column :stocks, :has_currant_ratio
  end
end
