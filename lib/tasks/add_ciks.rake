# Takes a stock ticker to get data for as an argument
namespace :stock do
  desc "Add cik number to each stock marked as 'listed'. Will skip over listed stocks which have a cik. NOTE: can add additional look up of ciks as described under 'TODO'"

  task :add_cik => :environment do |task, args|
    require 'active_record'
    require 'nokogiri'
    require 'open-uri'
    require 'uri'

    log = File.new("add_cik.log","w+")

    stocks = Stock.all.select{ |s| s.listed == true}

    stocks.each do |stock|

      ticker = stock.ticker

      if !stock.cik.nil? && stock.cik > 0
        puts "skipping #{ticker} because it already has cik #{stock.cik}"
        next
      end

      url = "http://www.sec.gov/cgi-bin/browse-edgar?CIK=#{ticker}&Find=Search&owner=exclude&action=getcompany"

      puts "\n Getting cik for #{ticker}"

      begin
        doc = Nokogiri::HTML(open(url))
      rescue OpenURI::HTTPError => e
        log.puts "Could not open url: #{e.message} \n for ticker: #{ticker}"
        next
      end

      link_node = doc.css("div.companyInfo").xpath('.//a').first


      if link_node.nil?
        log.puts "Did not find cik number for #{ticker} in link"
        name = stock.name
        log.puts "***Trying to get cik from company: #{stock.name}"

        # try getting cik from name lookup
        url = "http://www.sec.gov/cgi-bin/browse-edgar?company=#{URI.escape(stock.name)}&owner=exclude&action=getcompany"

        begin
          doc = Nokogiri::HTML(open(url))
        rescue OpenURI::HTTPError => e
          log.puts "Could not open url: #{e.message} \n for ticker: #{ticker}"
          next
        end

        link_node = doc.css("div.companyInfo").xpath('.//a').first

        if link_node.nil?
          log.puts "NOT succefull in getting cik"
          log.puts "looking up in table on edgar"

          url = "http://www.sec.gov/divisions/corpfin/organization/cfia-#{name[0].downcase}.htm"
          begin
            doc = Nokogiri::HTML(open(url))
          rescue OpenURI::HTTPError => e
            log.puts "Could not open url: #{e.message} \n for ticker: #{ticker}"
            next
          end
          # TODO:
          # can iterate over table of stock names untill we find the cik
          next

        else
          log.puts "SUCCESS! in getting cik from company name"
        end

      end # end getting link from other url

      link = link_node['href']

      if link.nil?
        puts "Link node did not contain a link for #{ticker}"
        next
      end

      cik = link[/CIK=(?<match>.*)/,"match"].to_i

      if cik != 0
        stock.update_attributes(:cik => cik)
        if stock.cik != cik
          log.puts "***Old cik was #{stock.cik}"
          puts "found cik mismatch for #{ticker}: old: #{stock.cik} new: #{cik}"
        end
        log.puts "Updated #{ticker} with cik #{cik}"
      else
        log.puts "Could not get cik for #{ticker}"
      end

    end #end iteration over all stocks

    log.close
  end
end

