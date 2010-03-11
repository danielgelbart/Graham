# == Schema Information
# Schema version: 20100309193254
#
# Table name: splits
#
#  id                       :integer         not null, primary key
#  stock_id                 :integer
#  old                      :integer
#  new                      :integer
#  split_mul_factor         :decimal(10, 2)
#  date                     :date
#  stock_adjusted_for_split :boolean
#  created_at               :datetime
#  updated_at               :datetime
#

class Split < ActiveRecord::Base
  belongs_to :stock
  after_create :adjust_stock!

  validates_uniqueness_of :date, :scope => :stock_id
  validates_presence_of :new, :old, :stock_id
  validates_numericality_of :new, :old


  def adjust_stock!
    ratio =  new.to_f / old.to_f
    update_attributes!(:split_mul_factor => ratio)

    transaction do
      stock.adjust_for_split!(self)
      update_attributes!(:stock_adjusted_for_split => true)
    end
  end

  def ratio_string
    "#{old}:#{new}"
  end

  # On model "my_stock"
  def adjust_for_split!(split)
    price = (buy_price / split.split_mul_factor)
    share_num = (shares * split.split_mul_factor)
    debugger
    update_attributes!(:buy_price => price, :shares => share_num)
  end



end
