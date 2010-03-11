class AddPortfolioIdToTransactions < ActiveRecord::Migration
  def self.up
    add_column :transactions, :portfolio_id, :integer
  end

  def self.down
    remove_column :transactions, :portfolio_id
  end
end
