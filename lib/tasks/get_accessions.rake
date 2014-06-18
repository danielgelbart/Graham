# Get edgar accesion numbers of annual reports on edgar
namespace :stock do
  desc "Get edgar accession numbers for annual reports"

  task :get_acns => :environment do |task, args|
    require 'active_record'
    require 'nokogiri'
    require 'open-uri'

    out_file = File.new("accession_numbers.txt","w+")
    out_file.puts("This is a list of accession numbers for annual reprots on Edgar\n")

    stocks = Stock.all.select{ |s| s.cik != nil }

    stocks.each do |stock|

      get_all_acns_for_stock(stock)

    end #iteration over stocks

    out_file.close
  end # task


 def get_all_acns_for_stock(stock)
   cik = stock.cik
   ticker = stock.ticker
   puts "Getting ACN for #{ticker}"
   out_file.puts("Name: "+ stock.name + " ; Ticker: "+ticker+" ; CIK: "+cik.to_s+"\n")

   url = "https://www.sec.gov/cgi-bin/browse-edgar?action=getcompany&CIK=#{cik}&type=10-k&dateb=&owner=exclude&count=40"

   begin
     doc = Nokogiri::HTML(open(url))
   rescue OpenURI::HTTPError => e
     puts "Could not open url: #{e.message}"
     puts "For ticker #{ticker}"
     out_file.puts("NO DATA\n")
     return
   end

   table = doc.css('div#seriesDiv').xpath('.//tr')

   table.shift # get rid of first tr in table wich is headers

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

     out_file.puts("YEAR "+ year +" ; ACN "+ acn +"\n")
     puts "Wrot acn for #{year}"
   end



 end




end #namespace :stock

