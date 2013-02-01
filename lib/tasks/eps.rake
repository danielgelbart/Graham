

# Takes a stock ticker to get data for as an argument
namespace :eps do
  desc "Load earnings per share data to database"
  task :get_data, [:ticker] => :environment do |task, args|
    require 'active_record'
    require 'nokogiri'
    require 'open-uri'
    ticker = args[:ticker]
    stock = Stock.find_by_ticker(ticker) 
 
    # get last five years earnings as 'diluted eps including extra items' from msn
     url = "http://investing.money.msn.com/investments/stock-income-statement/?symbol=US%3a#{ticker}"
    
    if stock.eps.size < 11
    
	start = 1 # how far back to get earnings
    	year = Time.new.year - 1
    	puts "\n Getting earnings records for #{ticker}"
    	doc = Nokogiri::HTML(open(url))
      
        eps = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first['id'] == "DilutedEPSIncludingExtraOrdIte" }  if doc
     
	if eps
	   puts "Getting last 5 year eps for #{ticker}"
     	   start = 6
     	   (1..5).each do |i|
	   	 if !eps.children[i].nil?
 	   	        ep = Ep.create(:stock_id => stock.id,
                	:year => year,
                	:eps =>  eps.children[i].text.to_f,
                	:source => url)
 	    		puts "created eps for #{stock.ticker}, year: #{ep.year}, eps: #{ep.eps}" 
            		year = year - 1
		 end
      	   end
        end

      # get eps for years 6-10 going back 
      if stock.eps.size < 7
      	url = "http://investing.money.msn.com/investments/financial-statements?symbol=US%3a#{ticker}"

      	doc = Nokogiri::HTML(open(url))

      	eps = doc.css('td:nth-child(6)') if !doc.nil?

      if eps.nil? || !eps.empty?
         (start..10).each do |i|
	     if !eps[i].nil?  	 
         	  ep = Ep.create(:stock_id => stock.id,
                	       		:year => year,
					:eps =>  eps[i].text.to_f,
                			:source => url)

        	  puts "created eps for #{stock.ticker}, year: #{ep.year}, eps: #{ep.eps}" if !ep.id.nil?
        	  year = year - 1
             end	
      	  end # do i loop
      end
      end # getting data 6-10 years back
    end # end of conditional if checking that eps already in database
  end
end


namespace :eps do
  desc "Load eps from past 10 years"
  task :get_many => :environment do |task, args|
  
      ss = Stock.all
      
      ss.each do |stock|
      #need to call reenable to call a rake task more than once
        Rake::Task["eps:get_data"].reenable
        Rake::Task["eps:get_data"].invoke(stock.ticker)
      end
  end
end
