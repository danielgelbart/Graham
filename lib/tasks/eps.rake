

# Takes a stock ticker to get data for as an argument
namespace :eps do
  desc "Load earnings per share data to database"
  task :get_data, :ticker, :needs => :environment do |task, args|
    require 'active_record'
    require 'nokogiri'
    require 'open-uri'
    ticker = args[:ticker]
    stock = Stock.find_by_ticker(ticker) || Stock.create(:ticker => ticker)
    url = "http://moneycentral.msn.com/investor/invsub/results/statemnt.aspx?Symbol=#{ticker}&lstStatement=10YearSummary&stmtView=Ann"

    puts "\n Getting earnings records for #{ticker}"
    doc = Nokogiri::HTML(open(url))

    eps = doc.css('td:nth-child(6)') if !doc.nil?

    if eps.nil? || !eps.empty?

      #get rid of first element
      eps.delete(eps.first)

      # the year is assumed by starting in 2009 and going back
      year = 2009

      eps.each do |e|
        ep = Ep.create(:stock_id => stock.id,
                :year => year,
                :eps =>  e.text.gsub(",","").to_f,
                :source => url)

        puts "created eps for #{stock.ticker}, year: #{ep.year}, eps: #{ep.eps}" if !ep.id.nil?
        year = year - 1
      end
    end
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
