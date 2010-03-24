# Includes methods for retriving finantial data of stocks including price, earnings per share, etc ...
module DataScraper
  require 'nokogiri'
  require 'open-uri'
  MILLION = 1000000
  BILLION = 1000000000

  def get_stock_price
    price = get_price_from_yahoo
    price = get_price_from_google if price.nil?
    price = get_price_from_msn if price.nil?
    price
  end

  def get_book_value
    book_value = get_book_value_from_msn
    book_value = get_book_value_from_yahoo if book_value.nil?
    book_value
  end

  def get_eps
    ttm_eps = get_eps_from_msn
    ttm_eps = get_eps_from_yahoo if ttm_eps.nil?
    ttm_eps
  end

  def get_sales
    get_sales_from_msn
  end

  def get_float
    get_float_from_yahoo
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
# book value scrapers --------------------------------------------------------
  def get_book_value_from_msn
    url = "http://moneycentral.msn.com/investor/invsub/results/hilite.asp?Symbol=US%3a#{ticker}"
    doc = open_url_or_nil(url)

    begin
       book_value = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Book Value/Share" }.xpath('./td').last.text.to_f
    rescue
    end
    book_value
  end


  def get_book_value_from_yahoo
    url = "http://finance.yahoo.com/q/ks?s=#{ticker}"
    doc = open_url_or_nil(url)

    begin
      book_value = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Book Value Per Share (mrq):" }.xpath('./td').last.text.to_f
    rescue
    end
    book_value
  end

# eps scrapers --------------------------------------------------------

  def get_eps_from_msn
    url = "http://moneycentral.msn.com/investor/invsub/results/hilite.asp?Symbol=US%3a#{ticker}"
    doc = open_url_or_nil(url)

    begin
      ttm_eps = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Earnings/Share" }.xpath('./td').last.text.to_f
      rescue
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
    ttm_eps
  end

  # sales scrapers -------------------------------------------------------
  def get_sales_from_msn
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
    sales.to_i
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
    begin
      mc = doc.xpath('//tr').detect{ |tr| tr.xpath('./th').first != nil && tr.xpath('./th').first.text == "Market Cap:" }.xpath('./td').text
    rescue
    end

    puts mc
    update_attributes!(:market_cap => mc)

  end


end

