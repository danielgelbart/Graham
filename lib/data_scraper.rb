# Includes methods for retriving finantial data of stocks including price, earnings per share, etc ...
module DataScraper
  require 'nokogiri'
  require 'open-uri'
#  require 'pry-debugger'

  include CommonDefs
  YEAR = Time.new.year

  # The above include covers the following three definitions
  # MILLION = 1000000
  # BILLION = 1000000000
  # YEAR = Time.new.year

  def scrape_data(mark = "0")

    # Mark as updated, so as not to download if not needed
    markn = mark
    last_mark = self.mark
    if !self.mark.nil? && !last_mark.match(markn).nil?
      puts "Skipping downloading data for #{ticker} - not going to download"
      return true
    end

    # Check if earnings data look strange, if so delete them
    earnings2012 = eps.select{|e| e.year == 2012 }.first
    earnings2011 = eps.select{|e| e.year == 2011 }.first
    condition = earnings2012 && earnings2011 && earnings2012.revenue == earnings2011.revenue && earnings2012.net_income == earnings2011.net_income
    if condition
      eps.map{ |s| s.delete }
      self.reload
      puts "[NOTE] earnings deleted for #{ticker} beacuse they apear to be erroneous"
    end

    # retriev:
    # From: gurufocus.com
    get_data_from_gurufocus # Retrievs: ta,tl, ca, cl, ltd, nta, bv
                            # Revenue, income, and eps
    get_numshares           # numshares - Not needed for first 1000 up to NU
    update_price
    get_dividends

    get_eps

    # mark stock as updated
    update_attributes( :mark => markn)
  end

  def yearly_update

    get_balance_sheets
    get_income
    get_book_value
    get_sales

    # get_dividends(1900) # get dividends as far back as possible

    get_last_year_eps_msn
    get_historic_eps(10) # get earnings for last year
    # get_eps # ttmeps

    #update_current_data # ttm_eps, sales, div_yield

    get_numshares

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

# Retrieve (at most) last 4 quarterly earnings and balance sheets
  def get_quarterly
    # get last year earnings
    l_year = latest_eps.year

    # get which quarters are the last 4
    fp = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Fiscal Period" }
    fp = fp.xpath('./td') if fp

    if fp.nil?
      puts "--------------------------------------Cannot get info for #{ticker}"
      return false
    end
    # Find last year by counting 'td's up to "TMM"
    years_available = 0 # Some stocks may not have 10 years worth of data
    for i in 1..fp.size
      if fp[i].nil? || !fp[i].text.match("TTM").nil?
        break
      end
      years_available = i
    end

    puts "Counted #{years_available} years of available data for #{ticker}"

    update_year = 1 # Some stocks may not be updated for 2012 yet
    update_year = 0 if fp[years_available].text.last == "2"




    #Acces data page
    url = "http://www.gurufocus.com/financials/#{ticker}"
    doc = open_url_or_nil(url)
    if doc.nil?
      puts "Could not get quarterly finantial data from gurufocus.com"
      return false
    end

    # Get last 4 quarters quarterly data
    # Check first if all 4 quarters are available?
    (1..4).each do |i|
      if fp[i].nil? || !fp[i].text.match("TTM").nil?
        break
      end
      years_available = i
    end

    puts "Counted #{years_available} years of available data for #{ticker}"

    update_year = 1 # Some stocks may not be updated for 2012 yet
    update_year = 0 if fp[years_available].text.last == "2"

    # A boolean to test if current asset values are available
    using_current_data = true

    # Scrape data from doc
    # Current Assets
    ca = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Total Current Assets" }
    if ca
      ca = ca.xpath('./td')
    else
      using_current_data = false
    end

    ta = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Total Assets" }
    ta = ta.xpath('./td') if ta

    cl = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Total Current Liabilities" }
    if cl
      cl = cl.xpath('./td')
    else
      using_current_data = false
    end

    tl = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Total Liabilities" }
    tl = tl.xpath('./td') if tl

    # Debt, book value, net tangible assets
    ltd = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Long-Term Debt" }
    ltd = ltd.xpath('./td') if ltd

    bv = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Total Equity" }
    bv = bv.xpath('./td') if bv

    ocs = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Other Current Assets" }
    ocs = ocs.xpath('./td') if ocs

    # Create balance sheet for 10 years
    (1..years_available).each do |i|
      cas = ""
      cls = ""
      ntas = ""
      if using_current_data
        cas = (clean_string(ca[i].text).to_f.round * MILLION).to_s
        cls = (clean_string(cl[i].text).to_f.round * MILLION).to_s
        if ocs
          ntas = (( clean_string(ca[i].text).to_f - clean_string(ocs[i].text).to_f - clean_string(cl[i].text).to_f ).round * MILLION ).to_s
        else
          ntas = cas
        end
      end

      # Some trusts don't have liabilities
      tler = ""
      tler = (clean_string(tl[i].text).to_f.round * MILLION).to_s if tl
      der = ""
      der = (clean_string(ltd[i].text).to_f.round * MILLION).to_s if ltd
      bver = ""
      bver = (clean_string(bv[i].text).to_f.round * MILLION).to_s if bv
      bs = BalanceSheet.create(:stock_id => self.id,
                            :year => YEAR - (years_available+1 - i) - update_year, #This reveses the year from i
                            :current_assets => cas,
                            :total_assets => (clean_string(ta[i].text).to_f.round * MILLION).to_s,
                            :current_liabilities => cls,
                            :total_liabilities => tler,
                            :long_term_debt => der,
                            :net_tangible_assets => ntas,
                            :book_value => bver,
                            :quarter => q)
        puts "Got bs data for #{ticker}, year: #{bs.year}, ta = #{bs.total_assets}" if !bs.id.nil?
    end

    update_attributes( :has_currant_ratio => using_current_data)

  end # end quarterly ---------------------------------------------



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

  def get_sharesnum
    get_numshares
  end

  def get_balance_sheets
    a = get_bs_from_msn
    get_balance_from_yahoo if a.nil?
  end

  def get_income
    get_revenue_income_msn
  end

  def get_book_value
    a = get_book_value_from_yahoo
    get_book_value_from_msn if a.nil?
    puts "#{a}"
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

  def get_price_from_msn(ticker)
    url = "http://www.msn.com/en-us/money/stockdetails/fi-126.1.#{ticker}"

    doc = open_url_or_nil(url)
    price = doc.css('.precurrentvalue') if doc
    if price
      begin
        price = clean_string( price.text ).to_f
      rescue
      end
    end
    price
  end

# Revenue and Incom scrapper from msn --------------------------------------
def get_revenue_income_msn
    year = Time.new.year
    puts "Getting Revenue and Income for #{ticker}"

    url = "http://investing.money.msn.com/investments/stock-income-statement/?symbol=US%3a#{ticker}"

    doc = open_url_or_nil(url)

    if doc
      tr = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "TotalRevenue" }
      tr = tr.xpath('./td') if tr

      ni = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "NetIncome" }
      ni = ni.xpath('./td') if ni
    else
      puts "Could not open URL"
    end

    # Msn gives numbers in millions, so we will multiply by 1000000
    if tr && ni

      (1..5).each do |i|
        # get eps for year
        is = eps.select{ |s| s.year.to_i ==  (year - i).to_i  }.first

        if is
          revenue = (clean_string(tr[i].text).to_f.round * MILLION).to_s
          income = (clean_string(ni[i].text).to_f.round * MILLION).to_s
          if is.update_attributes( :revenue => revenue, :net_income => income)
            puts "updated #{ticker} with revenue: #{revenue} and income #{income} for year #{ (year - i).to_s}"
          end
        else
          puts "eps not exist for #{ticker} for year #{ (year -i).to_s } and I am not going to create it"
        end
      end
    end

  end

# eps scrapers --------------------------------------------------------
# Just ttmeps

  def get_eps_from_msn
    url = "http://investing.money.msn.com/investments/financial-results/?symbol=us%3a#{ticker}"

    doc = open_url_or_nil(url)

    begin
      ttm_eps = doc.xpath('//tr//td')
      ttm_eps = ttm_eps[13].children[1].text.gsub(",","").to_f
    rescue
    end

    if ttm_eps
      update_attributes( :ttm_eps => ttm_eps )
      puts "eps for #{ticker} is #{ttm_eps}"
    else
      puts "Could not get ttm eps for #{ticker} from msn"
    end
      ttm_eps
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
    else
      puts "Could not get ttm eps for #{ticker} from yahoo"
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


  def get_data_from_gurufocus
    # Get data from MSN first, as it is more up to date, reliable, and correct
    # See STO in NOK for example


    #Acces data page
    url = "http://www.gurufocus.com/financials/#{ticker}"
    doc = open_url_or_nil(url)
    if doc.nil?
      puts "Could not get finantial data from gurufocus.com"
      return false
    end

    # Check if year is updated for 2012
    fp = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Fiscal Period" }
    fp = fp.xpath('./td') if fp

    if fp.nil?
      puts "--------------------------------------Cannot get info for #{ticker}"
      return false
    end
    # Find last year by counting 'td's up to "TMM"
    years_available = 0 # Some stocks may not have 10 years worth of data
    for i in 1..fp.size
      if fp[i].nil? || !fp[i].text.match("TTM").nil?
        break
      end
      years_available = i
    end

    puts "Counted #{years_available} years of available data for #{ticker}"

    update_year = 1 # Some stocks may not be updated for 2012 yet
    update_year = 0 if fp[years_available].text.last == (YEAR-1).to_s.last

    # A boolean to test if current asset values are available
    using_current_data = true

    # Scrape data from doc
    # Current Assets
    ca = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Total Current Assets" }
    if ca
      ca = ca.xpath('./td')
    else
      using_current_data = false
    end

    ta = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Total Assets" }
    ta = ta.xpath('./td') if ta

    cl = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Total Current Liabilities" }
    if cl
      cl = cl.xpath('./td')
    else
      using_current_data = false
    end

    tl = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Total Liabilities" }
    tl = tl.xpath('./td') if tl

    # Debt, book value, net tangible assets
    ltd = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Long-Term Debt" }
    ltd = ltd.xpath('./td') if ltd

    bv = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Total Equity" }
    bv = bv.xpath('./td') if bv

    ocs = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Other Current Assets" }
    ocs = ocs.xpath('./td') if ocs

    # Create balance sheet for 10 years
    (1..years_available).each do |i|
      cas = ""
      cls = ""
      ntas = ""
      if using_current_data
        cas = (clean_string(ca[i].text).to_f.round * MILLION).to_s
        cls = (clean_string(cl[i].text).to_f.round * MILLION).to_s
        if ocs
          ntas = (( clean_string(ca[i].text).to_f - clean_string(ocs[i].text).to_f - clean_string(cl[i].text).to_f ).round * MILLION ).to_s
        else
          ntas = cas
        end
      end

      # Some trusts don't have liabilities
      tler = ""
      tler = (clean_string(tl[i].text).to_f.round * MILLION).to_s if tl
      der = ""
      der = (clean_string(ltd[i].text).to_f.round * MILLION).to_s if ltd
      bver = ""
      bver = (clean_string(bv[i].text).to_f.round * MILLION).to_s if bv
      bs = BalanceSheet.create(:stock_id => self.id,
                            :year => YEAR - (years_available+1 - i) - update_year, #This reveses the year from i
                            :current_assets => cas,
                            :total_assets => (clean_string(ta[i].text).to_f.round * MILLION).to_s,
                            :current_liabilities => cls,
                            :total_liabilities => tler,
                            :long_term_debt => der,
                            :net_tangible_assets => ntas,
                            :book_value => bver )

        puts "Got bs data for #{ticker}, year: #{bs.year}, ta = #{bs.total_assets}" if !bs.id.nil?
    end

    update_attributes( :has_currant_ratio => using_current_data)

    #Scrape (from same page): Revenue, net earnings,
    # and - diluted EPS, including extra items
    r = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Revenue" }
    r = r.xpath('./td') if r

    ni = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Net Income" }
    ni = ni.xpath('./td') if ni

    # This is eps from gurufocus, not exactly what we want
    eps = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['title'] == "Earnings per Share (diluted) ($)" }
    eps = eps.xpath('./td') if eps


    (1..years_available).each do |i|
      cur_year = YEAR - (years_available+1 - i)- update_year
      # does an ep exist for this year?
      cur_ep = self.eps.detect{ |e| e.year == cur_year }
      if cur_ep.nil?
        # next if eps.nil?
        ep = Ep.create(:stock_id => self.id,
                     :year => cur_year,
                     :net_income => (clean_string(ni[i].text).to_f.round * MILLION).to_s,
                     :revenue => (clean_string(r[i].text).to_f.round * MILLION).to_s,
                     :eps => clean_string(eps[i].text).to_f,
                     :source => url)
        if !ep.id.nil?
          puts "Created new ep for #{ticker}, year: #{ep.year}, rev: #{ep.revenue}, income: #{ep.net_income}, eps: #{ep.eps.to_s}"
        end
      else
        puts "Found ep data for year #{cur_year}"
        if cur_ep && cur_ep.net_income.nil?
          cur_ep.update_attributes(:net_income => (clean_string(ni[i].text).to_f.round * MILLION).to_s,
                     :revenue => (clean_string(r[i].text).to_f.round * MILLION).to_s,
                     :source => url)
          puts "Updated ep data for #{ticker}, year: #{cur_ep.year}, rev: #{cur_ep.revenue}, income: #{cur_ep.net_income}"
        end
      end
      #puts "Got eps data for #{ticker}, year: #{YEAR - (11 - i)- update_year}, rev: #{r[i].text}, income: #{ni[i].text}, eps: #{eps[i].text}"
    end

    # MSN -- as data source from here on:

    # update eps for past 5 years from msn
    # get last five years earnings as 'diluted eps including extra items' from msn
    url = "http://investing.money.msn.com/investments/stock-income-statement/?symbol=US%3a#{ticker}"

    doc = open_url_or_nil(url)

    if doc.nil?
      puts "ERROR: Could not get data from msn for #{ticker}"
      return
    end
    puts "\n Updating eps data from msn for #{ticker}"

    #test if updated for 2013 or not (not neceraly updated to same year as gurufocus)
    last_year = YEAR - 1
    date = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "PeriodEndingDate" } if doc
    if date
      date = date.xpath('./td')
    else
      puts "cannot get information from msn for #{ticker}"
      return
    end

    update_year_msn = 1 #Some stocks may not be updated for 2013 yet
    update_year_msn = 0 if !date[1].text.match(last_year.to_s).nil?

    epss = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "DilutedEPS" }  if doc

    # if new model data was updated, we need to reload the cache
    self.reload

    # Update earnings from msn, if needed and available?
    if latest_eps.year == (last_year-1) && update_year_msn == 0
     puts "Creating new eps for year #{last_year}"
     rev2012 = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "TotalRevenue" }
     rev2012 = rev2012.xpath('./td')[1]
     earn2012 = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "NetIncome" }
     earn2012 = earn2012.xpath('./td')[1]
     eps2012 = epss.xpath('./td')[1]
     ep = Ep.create(:stock_id => self.id,
                     :year => last_year,
                     :revenue => (clean_string(rev2012.text).to_f.round * MILLION).to_s,
                     :net_income => (clean_string(earn2012.text).to_f.round * MILLION).to_s,
                     :eps => clean_string(eps2012.text).to_f, # Not that this might be overriden in the next code block
                     :source => url)
     if !ep.id.nil?
       puts "[From msn] Created new ep for #{ticker}, year: #{ep.year}, rev: #{ep.revenue}, income: #{ep.net_income}, eps: #{ep.eps.to_s}"
     end
   end

    if epss
      epss = epss.xpath('./td')

      #NOte - this updates last 5 year eps regardless of if exist
      (1..5).each do |i|
        if epss[i]
           year = YEAR - i - update_year_msn
           ep = self.eps.select{ |s| s.year == year }.first
           if !ep.nil?
             ep.update_attributes( :eps => clean_string(epss[i].text).to_f, :source => url)
             puts "Updated ep for #{ticker}, year: #{year}, eps: #{clean_string(epss[i].text)}"
           end
        end
      end
    end

    # If data is available for most recent past year, add this data for (2) Balance sheets
    if latest_balance_sheet.year == (YEAR-2) && update_year_msn == 0
      url = "http://investing.money.msn.com/investments/stock-balance-sheet/?symbol=us%3A#{ticker}&stmtView=Ann"
      doc = open_url_or_nil(url)

      if doc.nil?
        puts "ERROR: Could not get data from msn for #{ticker}"
        return
      end
      puts "\n Updating balance sheet data from msn for #{ticker} for year #{YEAR-1}"

      cas = cls = tler = der = ntas = bver = ""

      ca = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "CurrentAssets" }
      cas = ca.xpath('./td') if ca

      ta = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "TotalAssets" }
      ta = ta.xpath('./td') if ta

      cl = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "CurrentLiabilities" }
      if cl
        cls = cl.xpath('./td')
      else
        using_current_data = false
      end

      tl = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "TotalLiabilities" }
      tler = tl.xpath('./td') if tl

      # Debt, book value, net tangible assets
      ltd = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "LongTermDebt" }
      der = ltd.xpath('./td') if ltd

      bv = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "TotalEquity" }
      bver = bv.xpath('./td') if bv

      # Get the relavent string, and adjust it for saving in data base ONLY FOR 2012
      cas = (clean_string(cas[1].text).to_f.round * MILLION).to_s if cas != ""
      cls = (clean_string(cls[1].text).to_f.round * MILLION).to_s if cls != ""
      tler = (clean_string(tler[1].text).to_f.round * MILLION).to_s if tler != ""
      der = (clean_string(der[1].text).to_f.round * MILLION).to_s if der != ""
      ntas = cas
      bver = (clean_string(bver[1].text).to_f.round * MILLION).to_s if bver != ""

      bs = BalanceSheet.create(:stock_id => self.id,
                            :year => YEAR-1,
                            :current_assets => cas,
                            :total_assets => (clean_string(ta[1].text).to_f.round * MILLION).to_s,
                            :current_liabilities => cls,
                            :total_liabilities => tler,
                            :long_term_debt => der,
                            :net_tangible_assets => ntas,
                            :book_value => bver )

      puts "[From msn]Got bs data for #{ticker}, year: #{bs.year}, ta = #{bs.total_assets}" if !bs.id.nil?
      update_attributes( :has_currant_ratio => using_current_data)

    end #end of special update if got 2012 data from msn

  end #end of methos 'get_data_from_gurofucos' which realy gets data from msn


# only gets 5 years back
  def get_bs_from_msn

    if (balance_sheets.count >= 5) && (balance_sheets.detect{ |b| b.year == YEAR-1 } ) # no need to update
      puts "Balance sheets for #{ticker} up to date - not going to download"
      return true
    end

    url = "http://investing.money.msn.com/investments/stock-balance-sheet/?symbol=us%3A#{ticker}&stmtView=Ann"

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
  def get_dividends

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
    if !book_value_per_share.nil?
      puts "no need to update, book value is #{book_value_per_share} per share "
      return
    end
    url = "http://moneycentral.msn.com/investor/invsub/results/hilite.asp?Symbol=US%3a#{ticker}"
    doc = open_url_or_nil(url)
    return if doc.nil?
    begin
      book_value = doc.xpath('//tr').detect{ |tr| tr.xpath('./td')[2].text == "Book Value/Share" }.text.split("%").last.split("are").last.to_f
    rescue
    end
    if book_value
      update_attributes(:book_value_per_share => book_value)
      puts "updated book value for #{ticker} to #{book_vlue}"
    else
      puts "Was unable to get book value for #{ticker}"
    end
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
      begin
        mc = doc.xpath('//tr').detect{ |tr| tr.xpath('./th').first != nil && tr.xpath('./th').first.text == "Market Cap:" }.xpath('./td').text
      rescue
        mc = "" # Invalid date, skip to next
      end
    end

    update_attributes!(:market_cap => mc)
  end

def get_historic_eps(years_back)

    # get last five years earnings as 'diluted eps including extra items' from msn
    url = "http://investing.money.msn.com/investments/stock-income-statement/?symbol=US%3a#{ticker}"

    # dont download if historic eps data already exists
    # return if self.eps.count >= years_back

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


 # get number of shares outstanding

  def get_numshares

    puts "Getting shares outstanding for #{ticker}"

    url = "http://investing.money.msn.com/investments/financial-statements?symbol=US%3a#{ticker}"

    doc = open_url_or_nil(url)
    return if doc.nil?

    sel = doc.css('table[class = " mnytbl"]')
    return if sel.nil? or sel[1].nil?

    so = sel[1].css('td[class = "nwrp last"]')
    return if so.nil?

    year = YEAR - 1

    if !so.nil? && !so.empty?
      so.each do |n|
        if !n.nil?
          shares_outstanding = clean_string(n.text)
          next if shares_outstanding.to_i == 0
          sharec = Numshare.create(:stock_id => self.id,
                          :year => year,
                          :shares => shares_outstanding)
          puts "created numshare for #{ticker}, year: #{sharec.year}, shares: #{sharec.shares}" if !sharec.id.nil?
          year = year - 1
        end
      end
    end

  end # method for number of shares


  def get_dividendsm

    markn = "mark5"
    return if self.mark == markn

    # Some nasdaq stocks had " " (empty space) at end of name -> correct that:
    if ticker.last == " "
      new_tick = ticker.chop
      update_attributes( :ticker => new_tick )
    end

    url = "http://www.nasdaq.com/symbol/#{ticker}/dividend-history"

    puts "\n Getting dividends for #{ticker}"

    doc = open_url_or_nil(url)
    return if doc.nil?

    # get table with id: table id="dividendhistoryGrid"
    # Better way to do this with css selectors?
    dividends = doc.xpath('//table[@id="dividendhistoryGrid"]/tr').map{ |row| row.xpath('.//td')}
    # Now, dividends[i] is a row
    # dividends[i].children[1] is date of div i
    # dividends[i].children[5] is amount of div i

    #.map {|item| item.text.gsub!(/[\r|\n]/,"").strip} }
    # Get the right date format: Date.strptime("2/10/2012", '%m/%d/%Y')

    #remove first row
    dividends.delete_at(0)

    dividends.each do |d|
      begin
        Date.strptime( d[0].text.gsub!(/[\r|\n]/,"").strip , '%m/%d/%Y')
      rescue
        next # Invalid date, skip to next
      end

      div = Dividend.create(:stock_id => self.id,
                            :date => Date.strptime( d[0].text.gsub!(/[\r|\n]/,"").strip , '%m/%d/%Y'),
                            :amount => d[2].text.gsub!(/[\r|\n]/,"").strip.to_f,
                            :source => url)

      puts "\n Added dividend record for #{ticker}: date - #{ div.date }, amount: #{div.amount.to_f}" if !div.id.nil?
    end

    # Mark stock as handled:
    update_attributes(:mark => markn)

  end

end # end module datascraper
