

# Takes a stock ticker to get data for as an argument
namespace :eps do
  desc "Load earnings per share data to database"
  task :get_data, :ticker, :needs => :environment do |task, args|
    require 'active_record'
    require 'nokogiri'
    require 'open-uri'
    ticker = args[:ticker]
    stock = Stock.find_by_ticker(ticker) || Stock.create(:ticker => ticker)
 
    # get last five years earnings as 'diluted eps including extra items' from msn

    url = "http://moneycentral.msn.com/investor/invsub/results/statemnt.aspx?lstStatement=Income&symbol=US%3a#{ticker}&stmtView=Ann"
    
    if stock.eps.size < 10
    
	start = 1 # how far back to get earnings
    	year = Time.new.year - 1
    	puts "\n Getting earnings records for #{ticker}"
    	doc = Nokogiri::HTML(open(url))
      
        eps = doc.xpath('//tr').detect{ |tr| tr.xpath('./td').first != nil && tr.xpath('./td').first.text == "Diluted EPS Including Extraordinary Items" }  if doc
     
	if eps
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

      	url = "http://moneycentral.msn.com/investor/invsub/results/statemnt.aspx?lstStatement=10YearSummary&symbol=US%3a#{ticker}&stmtView=Ann"
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

    end # end of conditional if checking that eps already in database
  end
end


namespace :eps do
  desc "Load eps from past 10 years"
  task :get_many => :environment do |task, args|
    file = File.open("sp500.txt","r")
    line = file.gets #throw away first line
    while (line = file.gets)
      ticker = line.split().first
      name = line.gsub(ticker,"").strip
      stock = Stock.find_by_ticker(ticker) || Stock.create(:ticker => ticker, :name => name.gsub(/\n/,""))

      #need to call reenable to call a rake task more than once
      # perhaps this should be called as a method?
      # task :get_data do
      # ["MMM", "C"].each { |t| do task for t }
      #end

      Rake::Task["eps:get_data"].reenable
      Rake::Task["eps:get_data"].invoke(ticker)
    end
    file.close
  end
end
