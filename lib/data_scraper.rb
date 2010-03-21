# Includes methods for retriving finantial data of stocks including price, earnings per share, etc ...
module DataScraper
  require 'nokogiri'
  require 'open-uri'

  def get_stock_price(ticker)
    price = get_price_from_yahoo(ticker)
    price = get_price_from_goole(ticker) if price.nil?
    price = get_price_from_msn(ticker) if price.nil?
  end

  def open_url_or_nil(url)
    begin
      doc = Nokogiri::HTML(open(url))
    rescue OpenURI::HTTPError => e
      nil
    else
      doc
    end
  end

  def get_price_from_yahoo(ticker)
    url = "http://finance.yahoo.com/q?s=#{ticker}"
    doc = open_url_or_nil(url)

    if doc && doc.xpath('//tr')
      tr = doc.xpath('//tr').detect{ |tr| tr.xpath('./th').first != nil && tr.xpath('./th').first.text == "Last Trade:" }
      if tr
        price = tr.xpath('./td').last.text.to_f
      end
    end
    price
  end

  def get_price_from_google(ticker)
    url = "http://www.google.com/finance?q=#{ticker}"
    doc = open_url_or_nil(url)

    if doc
      div = doc.css('#price-panel')
      if div
        spans = div.xpath('./div/span')
        if spans
          price = spans.first.text.to_f
        end
      end
    end
    price
  end

  def get_price_from_msn(ticker)
    url = "http://moneycentral.msn.com/detail/stock_quote?Symbol=#{ticker}&getquote=Get+Quote"
    doc = open_url_or_nil(url)

    if doc
      td = doc.css('#detail')
      if td
        spans = td.xpath('./table/tbody/tr/th')
        if spans
          price = spans.first.text.to_f
        end
      end
    end
    price
  end

end
