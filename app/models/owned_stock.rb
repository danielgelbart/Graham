# == Schema Information
#
# Table name: owned_stocks
#
#  id           :integer(4)      not null, primary key
#  portfolio_id :integer(4)
#  stock_id     :integer(4)
#  shares       :integer(4)
#  created_at   :datetime
#  updated_at   :datetime
#

class OwnedStock < ActiveRecord::Base
  belongs_to :stock
  belongs_to :portfolio
  delegate :name, :ticker, :price, :ten_year_eps, :to => :stock

  validates_presence_of :stock_id, :shares, :portfolio_id








end

