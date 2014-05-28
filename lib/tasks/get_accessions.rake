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

        acn = tds[2].text[/.*Acc-no:([^(3]*)/,1] #No idea how this regex works

        year = tds[3].text.first(4).to_i - 1

        out_file.puts("YEAR "+ year.to_s +" ; ACN "+ acn +"\n")
        puts "Wrot acn for #{year}"
      end

    end #iteration over stocks

    out_file.close
  end
end

