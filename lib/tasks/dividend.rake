# This task rips historacle dividend data from the net and stores it to the db.
# current source of data: www.dividend.com



# Takes a stock ticker to get data for as an argument
namespace :dividend do
  desc "Load all past dividends to database"
  task :get_data, :ticker, :needs => :environment do |task, args|
    require 'active_record'
    require 'nokogiri'
    require 'open-uri'
    ticker = args[:ticker]
    stock = Stock.find_by_ticker(ticker) || Stock.create(:ticker => ticker)
    url = "http://www.dividend.com/historical/stock.php?symbol=#{ticker}"

    puts "\n Getting dividends for #{ticker}"
    begin
      doc = Nokogiri::HTML(open(url))
    rescue
    else

      # Better way to do this with css selectors?
      dividends = doc.xpath('//tr').map {
        |row| row.xpath('.//td/text()').map {|item| item.text.gsub!(/[\r|\n]/,"").strip} }


      #remove th row
      dividends.delete_at(0)
      dividends.delete_at(0)

      dividends.each do |d|
        div = Dividend.create(:stock_id => stock.id,
                            :date => d.first.to_date,
                            :amount => d.last.delete!('$').to_f,
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
