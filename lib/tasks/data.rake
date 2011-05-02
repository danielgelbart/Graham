# Takes a stock ticker to get data for as an argument
namespace :stock do
  desc "Load recent financial data"
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

    puts "Updated #{ticker}: book value/share: #{stock.book_value_per_share}, ttm_eps :#{stock.ttm_eps}" if success
    puts "book value/share: #{stock.book_value_per_share}, ttm_eps :#{stock.ttm_eps}" if !success

  end
end


namespace :stock do
  desc "Load eps from past 10 years"
  task :get_many => :environment do |task, args|
      ss = Stock.all
      ss.each do |stock|

         #need to call reenable to call a rake task more than once
    	 # perhaps this should be called as a method?
    	 # task :get_data do
     	 # ["MMM", "C"].each { |t| do task for t }
     	 #end

         Rake::Task["stock:get_data"].reenable
    	 Rake::Task["stock:get_data"].invoke(stock.ticker)
      end
   end
end

namespace :stock do
  desc "update balance sheets and other data"
  task :yearly_update => :environment do |task, args|
    require 'active_record'
    ss = Stock.all
    ss.map{ |s| s.yearly_update }
  end
end

namespace :stock do
  desc "get the earnings per share for the last year"
 task :get_last_eps => :environment do |task, args|
    require 'active_record'
    ss = Stock.all
    ss.map{ |s| s.get_last_y_eps }
  end
end
