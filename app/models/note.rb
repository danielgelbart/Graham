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
