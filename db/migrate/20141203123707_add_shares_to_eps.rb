class AddSharesToEps < ActiveRecord::Migration
  def change
    add_column :eps, :shares, :string
  end
end
