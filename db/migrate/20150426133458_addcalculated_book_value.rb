class AddcalculatedBookValue < ActiveRecord::Migration
  def up
    add_column :balance_sheets, :calculated_bv, :boolean, deafault:false
    add_column :balance_sheets, :calculated_tl, :boolean, deafault:false
  end

  def down
    remove_column :balance_sheets, :calculated_bv
    remove_column :balance_sheets, :calculated_tl
  end
end
