# == Schema Information
#
# Table name: searches
#
#  id              :integer(4)      not null, primary key
#  no_losses       :boolean(1)
#  divs            :boolean(1)
#  dilution        :float
#  created_at      :datetime
#  updated_at      :datetime
#  good_balance    :boolean(1)
#  book            :integer(4)
#  current_div     :boolean(1)
#  earning_growth  :boolean(1)
#  defensive_price :boolean(1)
#  big_enough      :boolean(1)
#  market_cap      :string(255)
#  sort_by         :string(255)
#  revenue         :string(255)
#  net_income      :string(255)
#  margin          :integer(4)
#  roe             :integer(4)
#

class Search < ActiveRecord::Base

# This method returns a list of stock per search request
  def stocks(sort_by = "pe_10")

    # this works very slowly
    # Maybe the best way to speed it up is to run the calculations, off line,
    # and then write the results to the db :)
    if @retrieved.nil?
       @stocks = Stock.where(listed: true)

      if defensive_price
        @stocks = @stocks.select{ |s| s.cheap? }
      end

      if !market_cap.nil?
        @stocks = @stocks.select{ |s| s.market_cap.to_i > market_cap.to_i }
      end


      if no_losses
        @stocks = @stocks.select{ |s| s.no_earnings_deficit? }
      end

      if good_balance
        @stocks = @stocks.select{ |s| s.financialy_strong? }
      end

      if divs
        @stocks = @stocks.select{ |s| s.continous_dividend_record? }
      end

      if current_div
        @stocks = @stocks.select{ |s| s.pays_dividends }
      end

      if dilution
        @stocks = @stocks.select{ |s| s.dilution(10) < (1+dilution/100) }
      end


      if book != 0
        if book > 0
          @stocks = @stocks.select{ |s| s.book_value > 0 }
        else
          @stocks = @stocks.select{ |s| s.book_value < 0 }
        end
      end

      if big_enough
        @stocks = @stocks.select{ |s| s.big_enough? }
      end

      if earning_growth
        @stocks = @stocks.select{ |s| s.eps_growth? }
      end

      has_nil_recs = true

      if !revenue.nil?
        @stocks.select!{ |s| ! s.ttm_earnings_record.nil? } if has_nil_recs
        has_nil_recs = false
        @stocks.select!{ |s| s.ttm_earnings_record.revenue.to_i > revenue.to_i }
      end

      if !net_income.nil?
        @stocks.select!{ |s| ! s.ttm_earnings_record.nil? } if has_nil_recs
        has_nil_recs = false
        @stocks.select!{ |s| s.ttm_earnings_record.net_income.to_i > net_income.to_i }
      end

      if !margin.nil?
        @stocks.select!{ |s| ! s.ttm_earnings_record.nil? } if has_nil_recs
        has_nil_recs = false
        @stocks.select!{ |s| s.ttm_earnings_record.net_income.to_f / s.ttm_earnings_record.revenue.to_i > (margin.to_f / 100) }
      end

      if !roe.nil?
        @stocks.select!{ |s| ! s.ttm_earnings_record.nil? } if has_nil_recs
        has_nil_recs = false
        @stocks.select!{ |s| s.ttm_earnings_record.net_income.to_f / s.book_value.to_i > (roe.to_f/100) }
      end

       @retrieved = 1
    end # end retrieving records

    # sort the results
   case sort_by
   when "pe10"
     @stocks = @stocks.sort_by{ |s| s.ten_year_eps }
   when "price_to_limit"
    @stocks = @stocks.sort_by{ |s| s.price_to_limit_ratio }
   when "price_to_book"
    @stocks = @stocks.sort_by{ |s| s.price_to_book_ratio }
   when "market_cap"
     @stocks = @stocks.sort_by{ |s| s.market_cap }
   when "pe"
     @stocks = @stocks.sort_by{ |s| s.pe }
   else
    @stocks = @stocks.sort_by{ |s| s.ten_year_eps }
   end

  end #method stocks

end #class search



