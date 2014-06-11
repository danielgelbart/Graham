# This file should contain all the record creation needed to seed the database with its default values.
# The data can then be loaded with the rake db:seed (or created alongside the db with db:setup).
#
# Examples:
#
#   cities = City.create([{ :name => 'Chicago' }, { :name => 'Copenhagen' }])
#   Major.create(:name => 'Daley', :city => cities.first)
User.seed(:id, :name) do |s|
      s.name = "Dani"
      s.id = 1
end

Portfolio.seed(:id) do |s|
  s.id = 1
  s.user_id = 1
  s.name = "A"
end

# Add a couple stocks, then get data for them
Stock.seed_many(:ticker,
                [{:name => "Berkshire Hathaway", :ticker => "BRK.A"},
                 {:name => "IBM", :ticker => "IBM"},
                 {:name => "CVX", :ticker => "CVX"},
                 {:name => "Teva Pharmasutical industries.", :ticker => "TEVA"}])

# Create NYSE stocks from a list maintained in 'nyse_stocks_list.txt'
=begin
file = File.open("nyse_stocks_list.txt","r")


while (line = file.gets)
  ticker = line.split.first
  name = line.split("\t").last.chop
  s = Stock.create(:ticker => ticker, :name => name)
  if !s.id.nil?
    puts "created #{s.ticker}"
    s.scrape_data
  end
end

file.close
=end #of seedin all stocks
