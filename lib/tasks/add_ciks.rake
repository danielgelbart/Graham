# Takes a stock ticker to get data for as an argument
namespace :stock do
  desc "Add cik number to each stock"

  task :add_cik => :environment do |task, args|
    require 'active_record'
    require 'nokogiri'
    require 'open-uri'

    er_file = File.new("list_of_stocks_with_no_cik.txt","w")
    er_file.puts("The following is a list of stocks I could not find the cik for:\n")
    stocks = Stock.all.select{ |s| s.listed == true}

    stocks.each do |stock|

      ticker = stock.ticker

      if !stock.cik.nil? && stock.cik > 0
        puts "skipping #{ticker} because it already has cik #{stock.cik}"
        next
      end

      url = "http://finance.yahoo.com/q/sec?s=#{ticker}+SEC+Filings"
      puts "\n Getting cik for #{ticker}"

      begin
        doc = Nokogiri::HTML(open(url))
      rescue OpenURI::HTTPError => e
        puts "Could not open url: #{e.message}"
        puts "For ticker #{ticker}"
        er_file.puts(ticker+"\n")
        next
      end

      link_node = doc.css("table#yfncsumtab").xpath('.//a').last

      if link_node.nil?
        puts "Did not find cik number for #{ticker}"
        er_file.puts(ticker+"\n")
        next
      end

      link = link_node['href']

      if link.nil?
        puts "Was unable to extract cik from link for #{ticker}"
        er_file.puts(ticker+"\n")
        next
      end

      cik = link[/cik=(?<match>.*)/,"match"].to_i

      if cik != 0
        stock.update_attributes(:cik => cik)
        puts "Updated #{ticker} with cik #{cik}"
      else
        puts "Could not get cik for #{ticker}"
        er_file.puts(ticker+"\n")
      end

    end #end iteration over all stocks

    er_file.close
  end
end

