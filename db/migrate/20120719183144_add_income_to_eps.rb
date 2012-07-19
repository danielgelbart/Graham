class AddIncomeToEps < ActiveRecord::Migration
  def self.up
    add_column :eps, :revenue, :string
    add_column :eps, :net_income, :string
  end

  def self.down
    remove_column :eps, :revenue
    remove_column :eps, :net_income
  end
end
