class AddDatesToDividends < ActiveRecord::Migration
  def change
    add_column :dividends, :pay_date, :date
    rename_column :dividends, :date, :ex_date
  end
end
