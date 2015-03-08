class AddEnumOptionsToNotes < ActiveRecord::Migration
  def up
    change_column :notes, :pertains_to, :enum, :limit => [:K10,:Q10,:REPORTS,:INCOME_REP,:BALANCE_REP,:COVER_REP, :ROYALTY_TRUST, :SHARES_OUTSTANDING ], :default => :REPORTS
  end

  def down
    change_column :notes, :pertains_to, :enum, :limit => [:K10,:Q10,:REPORTS,:INCOME_REP,:BALANCE_REP,:COVER_REP ], :default => :REPORTS
  end

end
