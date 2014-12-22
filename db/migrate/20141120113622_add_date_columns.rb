class AddDateColumns < ActiveRecord::Migration
  def up
    add_column :eps, :report_date, :datetime
  end

  def down
    remove_column :eps, :report_date
  end
end
