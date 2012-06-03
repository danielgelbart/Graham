# Includes methods for retriving finantial data of stocks including price, earnings per share, etc ...
module DataScraper
  require 'nokogiri'
  require 'open-uri'
  MILLION = 1000000
  BILLION = 1000000000
  YEAR = Time.new.year 

  def yearly_update
    get_balance_sheets
    # get_dividends(1900) # get dividends as far back as possible
    get_last_year_eps_msn
    get_historic_eps(10) # get earnings for last year
   # get_eps # ttmeps
   # get_sales
    update_current_data # ttm_eps, sales, div_yield
    update_price
  end

  def divs
     get_dividends(1900)
  end

# gets earings (eps) up to 10 years back
  def get_earnings
    get_historic_eps(1)
  end

  def daily_update
    update_price
  end

  def get_stock_price
    price = get_price_from_yahoo
    price = get_price_from_google if price.nil?
    price = get_price_from_msn if price.nil?
    price
  end

  def get_eps
    ttm_eps = get_eps_from_msn
    ttm_eps = get_eps_from_yahoo if ttm_eps.nil?
    ttm_eps
  end

  def get_last_y_eps
    get_last_year_eps_msn
  end

  def get_sales
    get_sales_from_msn
  end

  def get_balance_sheets
    a = get_bs_from_msn
    get_balance_from_yahoo if a.nil?
  end

  def get_book_value
    a = get_book_value_from_yahoo
    get_book_value_from_msn if a.nil?
  end


#  private

  def open_url_or_nil(url)
    begin
      doc = Nokogiri::HTML(open(url))
    rescue OpenURI::HTTPError => e
      nil
    else
      doc
    end
  end


  # Price scrapers -----------------------------------------------------------
  def get_price_from_yahoo
    url = "http://finance.yahoo.com/q?s=#{ticker}"
    doc = open_url_or_nil(url)

    begin
      price = doc.xpath('//tr').detect{ |tr| tr.xpath('./th').first != nil && tr.xpath('./th').first.text == "Last Trade:" }.xpath('./td').last.text.to_f if doc
    rescue
    end
    price
  end

  def get_price_from_google
    url = "http://www.google.com/finance?q=#{ticker}"
    doc = open_url_or_nil(url)

    begin
      price = doc.css('#price-panel').xpath('./div/span').first.text.to_f if doc
    rescue
    end
    price
  end

  def get_price_from_msn
    url = "http://moneycentral.msn.com/detail/stock_quote?Symbol=#{ticker}&getquote=Get+Quote"
    doc = open_url_or_nil(url)

    begin
      price = doc.css('#detail').xpath('./table/tbody/tr/th').first.text.to_f if doc
    rescue
    end
    price
  end


# eps scrapers --------------------------------------------------------
# for last year and create model
  def get_last_year_eps_msn
    return true if eps.detect{ |e| e.year == YEAR-1 }


    url = "http://moneycentral.msn.com/investor/invsub/results/statemnt.aspx?lstStatement=Income&Symbol=US%3a#{ticker}&stmtView=Ann"
    doc = open_url_or_nil(url)

    puts "#{ticker}"
    begin
      eps = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Diluted EPS Including Extraordinary Items" }.xpath('./td')[1].text.to_f
      rescue
    end
    if(eps)
      Ep.create(:stock_id => id,
                :year => YEAR - 1,
                :eps => eps,
                :source => url)
      puts "eps for #{YEAR - 1} was #{eps}"
    end
  end

# Just ttmeps

  def get_eps_from_msn
    url = "http://moneycentral.msn.com/investor/invsub/results/hilite.asp?Symbol=US%3a#{ticker}"
    doc = open_url_or_nil(url)

    begin
      ttm_eps = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Earnings/Share" }.xpath('./td').last.text.to_f
      rescue
    end
    if ttm_eps
      update_attributes( :ttm_eps => ttm_eps )
      puts "eps for #{ticker} is #{ttm_eps}"
    end
   end

  def get_eps_from_yahoo
    url = "http://finance.yahoo.com/q/ks?s=#{ticker}"
    doc = open_url_or_nil(url)

    begin
      ttm_eps = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Diluted EPS (ttm):" }.xpath('./td').last.text.to_f
    rescue
    end
    if ttm_eps
      update_attributes( :ttm_eps => ttm_eps )
      puts "eps for #{ticker} is #{ttm_eps}"
    end
  end

  # sales scrapers -------------------------------------------------------
  def get_sales_from_msn
    return false if latest_balance_sheet.nil?
    return true  if latest_balance_sheet.total_sales

    url = "http://moneycentral.msn.com/investor/invsub/results/hilite.asp?Symbol=US%3a#{ticker}"

    begin
      doc = open_url_or_nil(url)
      sales = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Sales" }.xpath('./td').last.text
      sales = case sales.split.last
              when  "Bil"
                sales.split.first.to_f * BILLION
              when  "Mil"
                sales.split.first.to_f * MILLION
              else
                sales.split.first.to_f * BILLION
              end
    rescue
    end

    if sales
      b = balance_sheets.detect{ |b| b.year == YEAR-1}
      if b
        b.update_attributes(:total_sales => sales )
        puts "sales for #{ ticker} #{YEAR-1} where #{sales}"
      end
    end

  end

  # float/shares outstanding scrapers ---------------------------------------

  # NOt working
  def get_float_from_yahoo
    url = "http://finance.yahoo.com/q/ks?s=#{ticker}"
    doc = open_url_or_nil(url)

    begin
      shares = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text.match(/Shares Outstanding/) }.xpath('./td').last.text
      puts shares
      shares = case shares.last
              when  "B"
                sales.chop.to_f * BILLION
              when  "M"
                sales.chop.to_f * MILLION
              else
                sales.chop.first.to_f * BILLION
              end

    rescue
    end
    shares.to_i
  end


  def get_ttm_div
    url = "http://finance.yahoo.com/q?s=#{ticker}"
    doc = open_url_or_nil(url)
    begin
      div = doc.xpath('//tr').detect{ |tr| tr.xpath('./th').first != nil && tr.xpath('./th').first.text == "Div & Yield:" }.xpath('./td').text
    rescue
    end

    if div
      ttm_div = div.split.first.to_f
      y = div.split.last.gsub(/[(|)|%]/,"").to_f

      update_attributes(:ttm_div => ttm_div,
                        :yield => y)
    end
  end

  def get_market_cap
    url = "http://finance.yahoo.com/q?s=#{ticker}"
    doc = open_url_or_nil(url)
    if doc
      mc = doc.xpath('//tr').detect{ |tr| tr.xpath('./th').first != nil && tr.xpath('./th').first.text == "Market Cap:" }.xpath('./td').text
    end

    update_attributes!(:market_cap => mc)
  end

  # Balace sheet ---------------------------------------------------------------
# only gets 5 years back
  def get_bs_from_msn

    if (balance_sheets.count >= 5) && (balance_sheets.detect{ |b| b.year == YEAR-1 } ) # no need to update
      puts "Balance sheets for #{ticker} up to date - not going to download"
      return true
    end

    url = "http://moneycentral.msn.com/investor/invsub/results/statemnt.aspx?Symbol=US%3a#{ticker}&lstStatement=Balance&stmtView=Ann"

    doc = open_url_or_nil(url)

    if doc
      ca = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "TotalCurrentAssets" }
      ca = ca.xpath('./td') if ca

      ta = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "TotalAssets" }
      ta = ta.xpath('./td') if ta

      cl = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "TotalCurrentLiabilities" }
      cl = cl.xpath('./td') if cl

      tl = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "TotalLiabilities" }
      tl = tl.xpath('./td') if tl

      ltd = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "TotalLongTermDebt" }
      ltd = ltd.xpath('./td') if ltd

      bv = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "TotalEquity" }
      bv = bv.xpath('./td') if bv
    else
      puts "Could not open URL"
    end

    # Msn gives numbers in millions, so we will multiply by 1000000
    if ca && cl
      #puts ca
      update_attributes!( :has_currant_ratio => true)

      (1..5).each do |i|
        BalanceSheet.create(:stock_id => self.id,
                            :year => YEAR - i,
                            :current_assets => (clean_string(ca[i].text).to_f.round * MILLION).to_s,
                            :total_assets => (clean_string(ta[i].text).to_f.round * MILLION).to_s,
                            :current_liabilities => (clean_string(cl[i].text).to_f.round * MILLION).to_s,
                            :total_liabilities => (clean_string(tl[i].text).to_f.round * MILLION).to_s,
                            :long_term_debt => (clean_string(ltd[i].text).to_f.round * MILLION).to_s,
                            :net_tangible_assets => (( clean_string(ca[i].text).to_f - clean_string(cl[i].text).to_f ).round * MILLION ).to_s,
                            :book_value => (clean_string(bv[i].text).to_f.round * MILLION).to_s )
      end

    else
      if ta && ltd # if could only retrive data for total assets and liabilities
        update_attributes!( :has_currant_ratio => false)
        (1..5).each do |i|
       #   puts "Adding - (total only) balance sheet for #{YEAR - i}"
          BalanceSheet.create(:stock_id => self.id,
                              :year => YEAR - i,
                              :total_assets => (clean_string(ta[i].text).to_f.round * MILLION).to_s,
                              :total_liabilities => (clean_string(tl[i].text).to_f.round * MILLION).to_s,
                              :long_term_debt => (clean_string(ltd[i].text).to_f.round * MILLION).to_s,
                              :book_value => (clean_string(bv[i].text).to_f.round * MILLION).to_s )
        end
      end
    end
  end


  def get_balance_from_yahoo
    if (balance_sheets.count >= 5) && (balance_sheets.detect{ |b| b.year == YEAR-1 } ) # no need to update
      puts "Balance sheets for #{ticker} up to date - not going to download"
      return true
    end

    url = "http://finance.yahoo.com/q/bs?s=#{ticker}&annual"
    doc = open_url_or_nil(url)
    puts ticker

    if doc

      ca = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Total Current Assets" }
      ca = ca.xpath('./td') if ca

      ta = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Total Assets" }
      ta = ta.xpath('./td') if ta

      cl = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Total Current Liabilities" }
      cl = cl.xpath('./td') if cl

      tl = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Total Liabilities" }
      tl = tl.xpath('./td') if tl

      ltd = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Long Term Debt" }
      ltd = ltd.xpath('./td') if ltd

      nta = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Total Stockholder Equity" }
      nta = nta.xpath('./td') if nta

      bv = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Net Tangible Assets" }
      bv = bv.xpath('./td') if bv


    end


    # Yahoo gives numbers in thousands, so we will add ,000
    ex = ",000"
    if ca && tl
      #puts ca
      update_attributes( :has_currant_ratio => true)
      (1..3).each do |i|
        if ca[i]
          BalanceSheet.create(:stock_id => self.id,
                              :year => YEAR- i,
                              :current_assets => (clean_string(ca[i].text) + ex).gsub(",",""),
                              :total_assets => (clean_string(ta[i].text) + ex).gsub(",",""),
                              :current_liabilities => (clean_string(cl[i].text) + ex).gsub(",",""),
                              :total_liabilities => (clean_string(tl[i].text) + ex).gsub(",",""),
                              :long_term_debt => (clean_string(ltd[i].text) + ex).gsub(",",""),
                              :net_tangible_assets => (clean_string(nta[i].text) + ex).gsub(/,|\$/,""),
                              :book_value => (clean_string(bv[i].text) + ex).gsub(",",""))
        end
      end
    else
      update_attributes!( :has_currant_ratio => false)
      puts "Trying to update total ratios from yahoo - but no one implemented that!"
    end

  end



  # Get dividends ------------------------------------------------------------

# Gets dividends as far back as (up to) the year supplied
  def get_dividends(year)

    if !dividends.empty?
      if dividends.sort_by{ |d| d.date }.last.date + 12.months  >  Time.new.to_date
        puts "dividends for #{ticker} up to date - not going to download"
        return true
      end
    end

    url = "http://dividata.com/stock/#{ticker}/dividend"
    puts "\n Getting dividends for #{ticker}"
    doc =  open_url_or_nil(url)

    if doc
      diveds = doc.xpath('//div[@id="divhisbotleft"]')

      diveds = diveds.children[5].children
  
      diveds.each do |d|
           next if d.nil?
           next if d.text.strip ==  "" # skip empty elements used as spacing by the website
           div = Dividend.create( :stock_id => self.id,
                                  :date => d.children[0].text.to_date,
                                  :amount => d.children[2].text.delete!('$').to_f,
                                  :source => url )

           puts "\n Added dividend record for #{ticker}: date - #{ div.date }, amount: #{div.amount.to_f}" if !div.id.nil?
    end
  end # if doc
end # method










# Is any of the following code  used?


# book value scrapers --------------------------------------------------------
  def get_book_value_from_msn
    url = "http://moneycentral.msn.com/investor/invsub/results/hilite.asp?Symbol=US%3a#{ticker}"
    doc = open_url_or_nil(url)

    begin
      book_value = doc.xpath('//tr').detect{ |tr| tr.xpath('./td')[2].text == "Book Value/Share" }.text.split("%").last.split("are").last.to_f
    rescue
    end
    update_attributes(:book_value_per_share => book_value) if book_value
  end


  def get_book_value_from_yahoo
    url = "http://finance.yahoo.com/q/ks?s=#{ticker}"
    doc = open_url_or_nil(url)

    begin
      book_value = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Book Value Per Share (mrq):" }.xpath('./td').last.text.to_f
    rescue
    end
      update_attributes(:book_value_per_share => book_value) if book_value
  end

    # Balace sheet ---------------------------------------------------------------

  def g_nc
    url = "http://finance.yahoo.com/q/bs?s=#{ticker}&annual"
    doc = open_url_or_nil(url)
    puts ticker

    if doc
      nta = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Net Tangible Assets" }
      nta = nta.xpath('./td') if nta
    end


    # Yahoo gives numbers in thousands, so we will add ,000
    ex = "000"


    if nta

     # puts nta

      bss = balance_sheets.sort_by{ |b| b.year * -1 }
      i = 1

      bss.each do |bs|
        bs.update_attributes(:net_tangible_assets =>  (clean_string(nta[i].text) + ex).gsub(/,|\$/,"") )
        i = i + 1
      end
    end
  end


  def clean_string(string)
    string.gsub(/\302|\240|,/,"").strip
  end



  def get_div_and_yield
     url = "http://finance.yahoo.com/q?s=#{ticker}"
    doc = open_url_or_nil(url)
    begin
      div = doc.xpath('//tr').detect{ |tr| tr.xpath('./th').first != nil && tr.xpath('./th').first.text == "Div & Yield:" }.xpath('./td').text
    rescue
    end

    if div
      ttm_div = div.split.first.to_f
      y = div.split.last.gsub(/[(|)|%]/,"").to_f

      update_attributes(:ttm_div => ttm_div,
                        :yield => y)
    end
  end

  def get_market_cap
    url = "http://finance.yahoo.com/q?s=#{ticker}"
    doc = open_url_or_nil(url)
    if doc
      mc = doc.xpath('//tr').detect{ |tr| tr.xpath('./th').first != nil && tr.xpath('./th').first.text == "Market Cap:" }.xpath('./td').text
    end

    update_attributes!(:market_cap => mc)
  end

def get_historic_eps(years_back)

    # get last five years earnings as 'diluted eps including extra items' from msn
    url = "http://investing.money.msn.com/investments/stock-income-statement/?symbol=US%3a#{ticker}"
  
    
    # dont download if historic eps data already exists
    return if self.eps.count >= years_back

  
    doc = open_url_or_nil(url)
	start = 1 # how far back to get earnings
    	year = YEAR - 1
    	puts "\n Getting earnings records for #{ticker}"

      
        eps = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "DilutedEPSIncludingExtraOrdIte" }  if doc
     
	if eps
          eps = eps.xpath('./td') 
          years_togo_back = min(5,years_back)
     	  (1..years_togo_back).each do |i|
	   	 if eps[i]
 	   	    ep = Ep.create(:stock_id => self.id,
                                   :year => YEAR - i,
                                   :eps => clean_string(eps[i].text).to_f,
                                   :source => url)
 	    		puts "created eps for #{ticker}, year: #{ep.year}, eps: #{ep.eps}"  if !ep.id.nil?
            	 end
      	   end
        end

      # get eps for years 6-10 going back 
      if years_back > 5 
        url = "http://investing.money.msn.com/investments/financial-statements?symbol=US%3a#{ticker}"

        doc = open_url_or_nil(url)

        eps = doc.css('td:nth-child(6)') if !doc.nil?

        year = year-5

        if !eps.nil? && !eps.empty?
          (5..years_back).each do |i|
	     if !eps[i].nil?  	 
         	  ep = Ep.create(:stock_id => self.id,
                	       	:year => year,
				:eps =>  clean_string(eps[i].text).to_f,
                		:source => url)

        	  puts "created eps for #{ticker}, year: #{ep.year}, eps: #{ep.eps}" if !ep.id.nil?
        	  year = year - 1
             end	
      	  end # do i loop
        end

      end # getting data 6-10 years back
  end # method for getting eps

end # end module datascraper
