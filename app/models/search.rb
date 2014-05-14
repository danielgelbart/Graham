class Search < ActiveRecord::Base
  

# This method returns a list of stock per search request
  def stocks
    @stocks = Stock.all

    if no_losses  
      @stocks = @stocks.select{ |s| s.no_earnings_deficit? } 
    end

    if divs
      @stocks = @stocks.select{ |s| s.continous_dividend_record? } 
    end
    
    if dilution
      @stocks = @stocks.select{ |s| s.dilution < (1+dilution/100) } 
    end
    
    @stocks = @stocks.sort_by{ |s| s.ten_year_eps }
  end  
end

# == Schema Information
#
# Table name: searches
#
#  id         :integer(4)      not null, primary key
#  no_losses  :boolean(1)
#  divs       :boolean(1)
#  dilution   :float
#  created_at :datetime
#  updated_at :datetime
#

