# == Schema Information
#
# Table name: eps
#
#  id             :integer(4)      not null, primary key
#  year           :integer(4)
#  source         :string(255)
#  eps            :decimal(12, 6)
#  stock_id       :integer(4)
#  created_at     :datetime
#  updated_at     :datetime
#  revenue        :string(255)
#  net_income     :string(255)
#  quarter        :integer(4)      default(0)
#  report_date    :datetime
#  shares         :string(255)
#  shares_diluted :boolean(1)      default(FALSE)
#  eps_diluted    :boolean(1)      default(FALSE)
#

class Ep < ActiveRecord::Base

  belongs_to :stock

  # validate uniquness of year And quarter in stock_id scope
  validates_uniqueness_of :year, :scope => [:stock_id, :quarter]
  validates_presence_of :stock_id

  def date_of
  # report_date unless report_date.nil?
    end_date = (year.to_s + "-" + stock.fiscal_year_end).to_date

    end_date += 1.years unless stock.fy_same_as_ed

    if quarter > 0
      return stock.newest_quarter.date_of if quarter == 5
      end_date -= (92*(4-quarter)).days
    end
    end_date
  end



  def margin
    if net_income && revenue
     net_income.to_f / revenue.to_f
    else
     0
    end
  end

  def shares_to_str
    return "-" if shares.to_i == 0
    billion = 1000000000.0
    if shares.to_i > billion
      return ("%5.2f" % (shares.to_i / billion) )+ " Bil"
    else
      return ("%5.2f" % (shares.to_i / 1000000.0) )+ " Mil"
    end
  end

end





