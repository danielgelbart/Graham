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

    doc = Nokogiri::HTML(open(url))


    # Better way to do this with css selectors?
    dividends = doc.xpath('//tr').map {
      |row| row.xpath('.//td/text()').map {|item| item.text.gsub!(/[\r|\n]/,"").strip} }


    #remove th row
    dividends.delete_at(0)
    dividends.delete_at(0)

    dividends.each do |d|
      div=Dividend.create(:stock_id => stock.id,
                      :date => d.first.to_date,
                      :amount => d.last.delete!('$').to_f,
                      :source => "www.dividend.com")
      puts "\ncreated: #{div.inspect}"
    end


  end
end
