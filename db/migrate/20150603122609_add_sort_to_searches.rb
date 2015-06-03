class AddSortToSearches < ActiveRecord::Migration
  def change
    add_column :searches, :sort_by, :string
  end
end
