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

end
