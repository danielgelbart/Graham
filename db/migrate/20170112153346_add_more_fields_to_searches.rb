class AddMoreFieldsToSearches < ActiveRecord::Migration
  def change
    add_column :searches, :revenue, :string
    add_column :searches, :net_income, :string
    add_column :searches, :margin, :integer
    add_column :searches, :roe, :integer
  end
end
