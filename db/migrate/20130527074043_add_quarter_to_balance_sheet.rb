class AddQuarterToBalanceSheet < ActiveRecord::Migration
  def self.up
    add_column :balance_sheets, :quarter, :integer, :default => 0
  end

  def self.down
    remove_column :balance_sheets, :quarter
  end
end
