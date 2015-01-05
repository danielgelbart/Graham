# This file should contain all the record creation needed to seed the database with its default values.
# The data can then be loaded with the rake db:seed (or created alongside the db with db:setup).
#
# Examples:
#
#   cities = City.create([{ :name => 'Chicago' }, { :name => 'Copenhagen' }])
#   Major.create(:name => 'Daley', :city => cities.first)

# Add a couple stocks, then get data for them


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
