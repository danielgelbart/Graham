class ConvertDatetimeToDateForEps < ActiveRecord::Migration
  def up
    change_column :eps, :report_date, :date
  end

  def down
    change_column :eps, :report_date, :datetime
  end
end
