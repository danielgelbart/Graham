module IConnection
  require 'nokogiri'
  require 'open-uri'

  def open_url_or_nil(url)
    begin
      resp = open(url)
      doc = Nokogiri::HTML(resp)
    rescue OpenURI::HTTPError => e
      nil
    else
      doc
    end
  end

  def goog_retrive_url(url)
#   logger.debug url
    doc = open_url_or_nil(url)
#   begin
      price = doc.css('#price-panel').xpath('./div/span').first.text if doc
      puts "Got prcie #{price}"
      price = clean_string(price).to_f
#   rescue
#   end
    puts "Cleaned price to  #{price}"
    price
  end

  def ya_retrive_url(url)
    #   logger.debug url
    doc = open_url_or_nil(url)
    begin
      price = doc.css('#quote-header-info').xpath('./div/div/span') if doc
     # puts "'price' text is #{price.text}"
      price = price.text
      price = clean_string(price).to_f
    rescue
    end
    #puts "Cleaned price to  #{price}"
    price
  end

  def clean_string(string)
    string.gsub(/\302|\240|,/,"").strip
  end

  def get_price(ex="",ticker="")
    #    get_price_from_google(ex,ticker)
    get_price_from_yahoo(ex,ticker)
  end

  def get_price_from_yahoo(ex="",ticker="")
    ticker = self.ticker if ticker == ""
    ticker = "BRK-A" if ticker == "BRK.A"
    ticker = "BRK-B" if ticker == "BRK.B"

    url = "https://finance.yahoo.com/quote/#{ticker}"
    price = ya_retrive_url(url)
  end

  def get_price_from_google(ex="",ticker="")
    #google redirecets "HP" to the company Hewlett Packard
    ex = "NYSE:" if (ticker == "HP")
    ex = "NYSE:" if (ticker == "H")
    ex = "NYSE:" if (ticker == "PNR")
    ex = "NYSE:" if (ticker == "UBP")
    ex = "NYSE:" if (ticker == "D")
    ex = "NYSE:" if (ticker == "IRM")
    ex = "NYSE:" if (ticker == "BHGE")
    ex = "NYSE:" if (ticker == "Q")
    ex = "NASDAQ:" if (ticker == "NWSA")
    ex = "NASDAQ:" if (ticker == "FOXA")
    ex = "NASDAQ:" if (ticker == "AAL")
    ex = "NASDAQ:" if (ticker == "CHTR")
    ex = "NASDAQ:" if (ticker == "SNPS")
    ex = "NASDAQ:" if (ticker == "MNST")
    #Google currently (Dec 2016) incorectly supplies UAA value for UA
    # These are two diff share classes. UA is class 'C'. (UAA is class 'A')
    ex = "NYSE:" if (ticker == "UA")

    url = "https://finance.google.com/finance?q=#{ex}#{ticker}"
    price = goog_retrive_url(url)

    if !price.nil? && price < 0.1
      ex = "NYSE:"
      url = "https://finance.google.com/finance?q=#{ex}#{ticker}"
      price = goog_retrive_url(url)
    end

    price
  end


end
