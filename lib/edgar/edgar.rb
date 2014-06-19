# this class is retrieves data from edgar.gov
class Acn
  attr_accessor :ticker, :year, :acn

  def initialize(ticker,year,acn)
    @ticker = ticker
    @year = year
    @acn = acn
    @cik = cik
  end

  def to_s
    "TICKER: "+ticker.to_s+" , YEAR: "+year.to_s+" , ACN: "+acn.to_s+"\n"
  end

  def url
    "http://www.sec.gov/Archives/edgar/data/#{cik}/#{acn}.txt"
  end


end


class AcnList
  attr_accessor :ticker, :name, :cik

  def initialize(stock)
    @name = stock.name
    @cik = stock.cik
    @ticker = stock.ticker
    @list = []
  end

  def add(acn)
    @list << acn
  end

  def to_s
    str = "NAME: "+name+" , TICKER: "+ticker+" , CIK: "+cik.to_s+"\n"
    @list.each do |rec|
      str += rec.to_s
    end
    str
  end

end

class Edgar

  attr_accessor :documents, :log, :stock

  def initialize(stock)
    self.stock = stock
  end

  def get_10k(year,acn)


  end

  def extract_finantials

  end

  def get_all_available_financials

  end



  def get_acns
    ticker = stock.ticker
    cik = stock.cik

    if cik.nil?
      puts "cannot get accesions without cik for #{ticker}"
      return
    end

    puts "Getting ACN for #{ticker}"
    #out_file.puts("Name: "+ stock.name + " ; Ticker: "+ticker+" ; CIK: "+cik.to_s+"\n")

    url = "https://www.sec.gov/cgi-bin/browse-edgar?action=getcompany&CIK=#{cik}&type=10-k&dateb=&owner=exclude&count=40"

    doc = open_doc(url)
    return if doc.nil?

    table = doc.css('div#seriesDiv').xpath('.//tr')
    table.shift # get rid of first tr in table wich is headers


    acns = AcnList.new(stock)
    table.each do |tr|

      tds = tr.xpath('./td')

      #check that this row is for a 10-k
      next if tds[0].text != "10-K"

      # Capture the accistion number from the string
      acn = tds[2].text[/.*Acc-no: (?<acn>(\d|\-)*)/,"acn"]

      year = tds[3].text.first(4).to_i - 1
      year = year.to_s

      #At this point we have all the data needed to retriev 10 Ks
      # We have: cik and acn
      # We can either 1) Write this info to a file for latter use
      #               2) Get the file and save it for latter use
      #               3) Get the file, parse it and save the finantial data

      # Consider changing this method to end with
      # return ticker, cik, acn, year

      #        get_data(cik,acn,year)


      acns.add(Acn.new(ticker,year,acn,cik))
#      out_file.puts("YEAR "+ year +" ; ACN "+ acn +"\n")
#      puts "Wrote acn for #{year}"
    end

    acns
  end

private

    def open_doc(url)
      begin
        doc = Nokogiri::HTML(open(url))
      rescue OpenURI::HTTPError => e
        puts "Could not open url: #{e.message}"
        puts "For ticker #{ticker}"
        #out_file.puts("NO DATA\n")
        return
      end
    end

end # class


