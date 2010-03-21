# Takes a stock ticker to get data for as an argument
namespace :stock do
  desc "Load recent finanyial data"
  task :get_data, :ticker, :needs => :environment do |task, args|
    require 'active_record'
    require 'nokogiri'
    require 'open-uri'
    ticker = args[:ticker]
    stock = Stock.find_by_ticker(ticker) || Stock.create(:ticker => ticker)
    url = "http://moneycentral.msn.com/investor/invsub/results/hilite.asp?Symbol=US%3a#{ticker}"

    puts "\n Getting data for #{ticker}"

    begin
      doc = Nokogiri::HTML(open(url))
    rescue OpenURI::HTTPError => e
      puts "Could not open url: #{e.message}"
    end

    if doc && doc.xpath('//tr')
      tr = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Earnings/Share" }

      if tr
        ttm_eps = tr.xpath('./td').last.text.to_f

        book_value = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Book Value/Share" }.xpath('./td').last.text.to_f
      end
    end

    if !book_value #Try yahoo for retriving data
      url = "http://finance.yahoo.com/q/ks?s=#{ticker}"
      begin
        doc = Nokogiri::HTML(open(url))
      rescue OpenURI::HTTPError => e
        puts "Could not open url: #{e.message}"
      else
        if doc && doc.xpath('//tr')
          tr = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Diluted EPS (ttm):" }
          if tr
            ttm_eps = tr.xpath('./td').last.text.to_f

            book_value = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Book Value Per Share (mrq):" }.xpath('./td').last.text.to_f
          end
        end
      end
    end
=begin
      #Get sales data
    url = "http://finance.yahoo.com/q/ks?s=#{ticker}"
    begin
      doc = Nokogiri::HTML(open(url))
    rescue OpenURI::HTTPError => e
    else
      tr = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Revenue (ttm):" }
      if tr
        revenue = tr.xpath('./td').last.text
        revenue = case revenue.last
                  when "B"
                    revenue.chop.to_i * 1000000000
                  when "M"
                    revenue.chop.to_i * 1000000
                  else
                    revenue.to_i
                  end
      end
    end
=end

    success = stock.update_attributes!(:ttm_eps => ttm_eps,
                                       :book_value_per_share => book_value,
                                       :finantial_data_updated => Date.today)

    puts "Updated #{ticker}: book value/share: #{stock.book_value_per_share}, ttm_eps :#{stock.ttm_eps}, revenue: #{stock.revenue}" if success
    puts "book value/share: #{stock.book_value_per_share}, ttm_eps :#{stock.ttm_eps}" if !success

  end
end


namespace :stock do
  desc "Load eps from past 10 years"
  task :get_many => :environment do |task, args|
    file = File.open("sp500.txt","r")
    line = file.gets #throw away first line
    while (line = file.gets)
      ticker = line.split().first
      name = name = line.gsub!(ticker,"").strip
      stock = Stock.find_by_ticker(ticker) || Stock.create(:ticker => ticker, :name => name.gsub(/\n/,""))

      #need to call reenable to call a rake task more than once
      # perhaps this should be called as a method?
      # task :get_data do
      # ["MMM", "C"].each { |t| do task for t }
      #end

      Rake::Task["stock:get_data"].reenable
      Rake::Task["stock:get_data"].invoke(ticker)
    end
    file.close
  end
end
