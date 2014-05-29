# Get edgar accesion numbers of annual reports on edgar
namespace :stock do
  desc "Get edgar accession numbers for annual reports"

  task :get_acns => :environment do |task, args|
    require 'active_record'
    require 'nokogiri'
    require 'open-uri'

    out_file = File.new("accession_numbers.txt","w")
    out_file.puts("This is a list of accession numbers for annual reprots on Edgar\n")

    stocks = Stock.all.select{ |s| s.cik != nil }

    stocks.each do |stock|

      cik = stock.cik
      ticker = stock.ticker
      puts "Getting ACN for #{ticker}"
      out_file.puts("Ticker: "+ticker+" ; CIK: "+cik.to_s+"\n")

      url = "https://www.sec.gov/cgi-bin/browse-edgar?action=getcompany&CIK=#{cik}&type=10-k&dateb=&owner=exclude&count=40"


      begin
        doc = Nokogiri::HTML(open(url))
      rescue OpenURI::HTTPError => e
        puts "Could not open url: #{e.message}"
        puts "For ticker #{ticker}"
        out_file.puts("NO DATA\n")
        next
      end

      table = doc.css('div#seriesDiv').xpath('.//tr')

      table.shift # get rid of first tr in table wich is headers

      table.each do |tr|

        tds = tr.xpath('./td')

        #check that this row is for a 10-k
        next if tds[0].text != "10-K"

        # TODO
        # THIS REGEX IS NOT GOOD ENOUGH
        # It eiterh chops the ac number to short, or adds an '_' at the end

        acn = tds[2].text[/.*Acc-no:([^(3|Si]*)/,1] #No idea how this regex works

        year = tds[3].text.first(4).to_i - 1
        year = year.to_s

        get_data(cik,acn,year)

        out_file.puts("YEAR "+ year +" ; ACN "+ acn +"\n")
        puts "Wrot acn for #{year}"
      end

    end #iteration over stocks

    out_file.close
  end # task


  def get_data(cik,acn,year)
    url = "http://www.sec.gov/Archives/edgar/data/#{cik}/#{acn}.txt"

    doc = get_doc(url)

    #1) Get:
    #1 total revenue
    #2 Net Income
    extract_income_statement(doc)

  end

  # get the doc from a url
  def get_doc(url)
    begin
      doc = Nokogiri::HTML(open(url))
    rescue OpenURI::HTTPError => e
      puts "Could not open url: #{e.message}"
    end
  end

  def extract_income_statement(doc)


  end


  # extract the rows of a relavent table
  # return an array or NokogiriXmlNodes representing the table rows
  def get_rows
  end

  # extract the year represented by the row
  def get_row_year

  end



end #namespace :stock

