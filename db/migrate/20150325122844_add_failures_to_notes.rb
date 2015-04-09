class AddFailuresToNotes < ActiveRecord::Migration

    def up
      change_column :notes, :pertains_to, :enum, :limit => [:K10,:Q10,:REPORTS,:INCOME_REP, :NO_REV, :NO_INC, :NO_SHARES, :NO_EPS, :BALANCE_REP,:COVER_REP, :SHARES_OUTSTANDING ], :default => :REPORTS
    end

    def down
      change_column :notes, :pertains_to, :enum, :limit => [:K10,:Q10,:REPORTS,:INCOME_REP,:BALANCE_REP,:COVER_REP, :SHARES_OUTSTANDING ], :default => :REPORTS
    end

end
