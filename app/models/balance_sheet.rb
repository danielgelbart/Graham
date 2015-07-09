# == Schema Information
#
# Table name: balance_sheets
#
#  id                  :integer(4)      not null, primary key
#  stock_id            :integer(4)
#  year                :integer(4)
#  current_assets      :string(255)
#  total_assets        :string(255)
#  current_liabilities :string(255)
#  total_liabilities   :string(255)
#  long_term_debt      :string(255)
#  book_value          :string(255)
#  created_at          :datetime
#  updated_at          :datetime
#  net_tangible_assets :string(255)
#  quarter             :integer(4)      default(0)
#  calculated_bv       :boolean(1)
#  calculated_tl       :boolean(1)
#
#bs = BalanceSheet.create(stock_id: s.id, year:2011, current_assets:"51893000000", total_assets:"180663000000", current_liabilities:"58484000000",total_liabilities:"109416000000", long_term_debt:"40692000000", book_value:"71247000000", calculated_tl: true )
class BalanceSheet < ActiveRecord::Base

  belongs_to :stock

  validates_presence_of :stock_id
  validates_uniqueness_of :year, :scope => :stock_id

  include CommonDefs

  def equity
    ret = book_value.to_i
    if ret == 0
     ret =  sum_to_i(total_assets) - sum_to_i(total_liabilities)
    end
    ret
  end

  def is_quarterly?
    quarter > 0
  end

  # rewrite this as a method that combines the attribute name and 'translate' method
  def assets_c
   translate_to_int(self.current_assets)
  end

  def assets_t
    translate_to_int(self.total_assets)
  end

  def liabilities_c
    translate_to_int(self.current_liabilities)
  end

  def liabilities_t
    translate_to_int(self.total_liabilities)
  end

  def book_val
    translate_to_int(self.book_value)
  end

  def debt
    translate_to_int(self.long_term_debt)
  end

  def ncav
    translate_to_int(self.net_tangible_assets)
  end

  def current_ratio
    return nil if assets_c.nil? || liabilities_c.nil?
    assets_c.to_f / liabilities_c.to_f
  end

  private

  def translate_to_int(str)
    if str.nil?
      return nil
    end
    val = str.gsub(/,|\$/,"")
    val = "-" + val.gsub(/\(|\)/,"") if val.match(/\(\$?\d+\)/)
    val.to_i
  end

end


