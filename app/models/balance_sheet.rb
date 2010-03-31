# == Schema Information
# Schema version: 20100331094540
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
#

class BalanceSheet < ActiveRecord::Base

  belongs_to :stock

  validates_presence_of :stock_id
  validates_uniqueness_of :year, :scope => :stock_id

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

  def book_value
    translate_to_int(self.book_value)
  end

  def debt
    translate_to_int(self.long_term_debt)
  end

  private


  def translate_to_int(str)
    val = str.gsub(",","").to_i
    if val > 0
      val
    else
      nil
    end
  end


end
