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
#  shares_diluted :boolean(1)      default(TRUE)
#

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
#  shares_diluted :boolean(1)      default(TRUE)
#
class Ep < ActiveRecord::Base

  belongs_to :stock

  # validate uniquness of year And quarter in stock_id scope
  validates_uniqueness_of :year, :scope => [:stock_id, :quarter]
  validates_presence_of :stock_id

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


