class AddCountryAndsharedilutedboolean < ActiveRecord::Migration
  def change
    add_column :stocks, :country, :string, :defualt => "USA"
    add_column :eps, :shares_diluted, :boolean, :default => true
  end
end
