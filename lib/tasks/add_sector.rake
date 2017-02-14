# Takes a stock ticker to get data for as an argument
namespace :stock do
  desc "Add sector information to stocks using list of stocks. If stock does not exist and has market cap of over 1B, add it to DB. make sure to get cik"

  task :add_sector => :environment do |task, args|
    require 'active_record'
    require 'csv'
    require 'nokogiri'
#    require 'open-uri'
#    require 'uri'

    log = File.new("add_sector.log","w+")
    counter = 0
    # Add to stocks: sector, industry, ipoyear

    # Make list of ALL sectors and ALL industries
    #
    #
    #
    h = {}

    # 0"Symbol",1"Name",2"LastSale",3"MarketCap",4"IPOyear",5"Sector",6"industry",7"Summary Quote"
    CSV.foreach(Rails.root.join('stock_lists', 'NASDAQ_companylist.csv')) do |row|
      ticker = row[0].strip
#      sector = row[5].strip
      industry = row[6].strip
      next if industry == "n/a"
      ipoyear = row[4].strip
      if ipoyear == "n/a"
        ipoyear = 0
      end
=begin
      if h[sector].nil?
        h.store(sector,[])
      end

      if ! h[sector].include? industry
        h[sector] << industry
      end

      # puts "#{ticker} Sector: #{sector} Industry: #{industry} IPO: #{ipoyear}"
      counter += 1

      if counter > 25
        break
      end
=end
      stock = Stock.get_from_ticker(ticker)


      if !stock.nil?
        next if ! stock.sub_sector.nil?

        sub_sec = SubSector.find_by_name(industry)
        if !sub_sec.nil?

          if stock.update_attributes( sub_sector: sub_sec, ipo_year: ipoyear)
            puts "Updating #{stock.ticker} with ssc: #{sub_sec.name}, ipo: #{ipoyear}"
          end
        end
      else
        marcap = row[3].strip
        if marcap
          # add to DB

          # get cik

          # mark as new, for later update price and financials
        end

      end


    end #csv file

=begin
    ic = 0
    puts "Logged #{h.size} sectors:"

    h.each do |s,ins|
      puts "SECTOR: #{s}, #{ins.size} ins:"
      next if s == "n/a"

      sec = Sector.find_by_name(s)
      if ! sec.nil?

        ins.each do |i|
          ssc = SubSector.create( sector:sec, name:i)
          puts "      Added sub sec: #{ssc.name}" if ! ssc.nil?
          ic += 1
        end
      end

    end

    puts "#{h.size} sectors, #{ic} industries"
=end


  end #task

end

