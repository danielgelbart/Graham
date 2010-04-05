class AddNetTangibles < ActiveRecord::Migration
  def self.up
    add_column :balance_sheets, :net_tangible_assets, :string
  end

  def self.down
    add_column :balance_sheets, :net_tangible_assets
  end
end
