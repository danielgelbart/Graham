class Numshare < ActiveRecord::Base
  belongs_to :stock
  validates_uniqueness_of :year, :scope => :stock_id

  include CommonDefs

  def shares_to_i
    case self.shares
      when /\d+\.\d+ Bil/
        (self.shares.to_f * BILLION).to_i
      when /\d+\.\d+ Mil/
        (self.shares.to_f * MILLION).to_i
      else
        (self.shares.to_f * BILLION).to_i
      end
  end

end
