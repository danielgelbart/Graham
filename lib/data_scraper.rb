# Includes methods for retriving finantial data of stocks including price, earnings per share, etc ...
module DataScraper
  require 'nokogiri'
  require 'open-uri'

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


  private

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

end

