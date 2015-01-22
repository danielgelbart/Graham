class Note < ActiveRecord::Base
  belongs_to :stock
#  validates_columns :pertains_to

  # for handling enums
  def self.enum_columns
    # can be made dynamic wite columns_hash.each{ |c| c.sql_type}
    [:pertains_to]
  end

  def self.enum_field?(col_name)
    return enum_columns.include?(col_name)
  end

  def self.enum_options(col_name)
    ar = Note.columns_hash[col_name.to_s].limit
    i = 0
    Hash[ar.map {|v| [v, i=i+1]}]
  end

end
