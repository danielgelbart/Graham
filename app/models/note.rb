# == Schema Information
#
# Table name: notes
#
#  id          :integer(4)      not null, primary key
#  stock_id    :integer(4)
#  year        :integer(2)      default(2014)
#  pertains_to :enum([:K10, :Q1 default(:REPORTS)
#  note        :string(255)
#  created_at  :datetime        not null
#  updated_at  :datetime        not null
#

# enum options:
# enum EnumNotePERTAINS_TO { K10 = 1, Q10 = 2, REPORTS = 3, INCOME_REP = 4, NO_REV = 5, NO_INC = 6, NO_SHARES = 7, NO_EPS = 8, BALANCE_REP = 9, COVER_REP = 10, SHARES_OUTSTANDING = 11

class Note < ActiveRecord::Base
  belongs_to :stock

  # for handling enums
  def self.enum_columns
    # can be made dynamic wite columns_hash.each{ |c| c.sql_type}
    [:pertains_to]
  end

  def self.enum_field?(col_name)
    return enum_columns.include?(col_name)
  end

  def self.enum_options(col_name)
    enum_col = Note.columns_hash[col_name.to_s]
    ar = enum_col.limit
    ar = enum_string_to_values_array(enum_col.sql_type) if ar.class == Fixnum
    i = 0
    Hash[ar.map {|v| [v, i=i+1]}]
  end

  def self.enum_string_to_values_array(str)
    str_arr = str[6..-3].split(/','/)
  end
end
