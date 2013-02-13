# This task rips historacle dividend data from the net and stores it to the db.
# current source of data: www.dividend.com



# Takes a stock ticker to get data for as an argument
namespace :dividend do
  desc "Load all past dividends to database"
  task :get_data, [:ticker] => :environment do |task, args|
    require 'active_record'
    require 'nokogiri'
    require 'open-uri'
    ticker = args[:ticker]
    stock = Stock.find_by_ticker(ticker) || Stock.create(:ticker => ticker)
    url = "http://www.nasdaq.com/symbol/#{ticker}/dividend-history"

    puts "\n Getting dividends for #{ticker}"
    begin
      doc = Nokogiri::HTML(open(url))
    rescue
    else
      # get table with id: table id="dividendhistoryGrid" 
      ('//table[@id="dividendhistoryGrid"]/tr')
      # Better way to do this with css selectors?
      dividends = doc.xpath('//table[@id="dividendhistoryGrid"]/tr').map{ |row| row.xpath('.//td')}
      # Now, dividends[i] is a row
      # dividends[i].children[1] is date of div i
      # dividends[i].children[5] is amount of div i
      
      #.map {|item| item.text.gsub!(/[\r|\n]/,"").strip} }
      # Get the right date format: Date.strptime("2/10/2012", '%m/%d/%Y')
      
      #remove first row
      dividends.delete_at(0)
      
      dividends.each do |d|
        div = Dividend.create(:stock_id => stock.id,
                            :date => Date.strptime( d[0].text.gsub!(/[\r|\n]/,"").strip , '%m/%d/%Y'),
                            :amount => d[2].text.gsub!(/[\r|\n]/,"").strip.to_f,
                            :source => url)

        puts "\n Added dividend record for #{ticker}: date - #{ div.date }, amount: #{div.amount.to_f}" if !div.id.nil?
      end
      
    end
  end
end


namespace :dividend do
  desc "Load all past dividends to database"
  task :get_many => :environment do |task, args|
    file = File.open("sp500.txt","r")
    line = file.gets #throw away first line
    while (line = file.gets)
      ticker = line.split("\t").first
      name = line.split("\t").last
      stock = Stock.find_by_ticker(ticker) || Stock.create(:ticker => ticker, :name => name.gsub(/\n/,""))

      #need to call reenable to call a rake task more than once
      # perhaps this should be called as a method?
      # task :get_data do
      # ["MMM", "C"].each { |t| do task for t }
      #end

      Rake::Task["dividend:get_data"].reenable
      Rake::Task["dividend:get_data"].invoke(ticker)
    end
    file.close
  end
end
