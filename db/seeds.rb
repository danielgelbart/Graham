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

#Share classes for stocks
stock = Stock.find_by_ticker("BRK.A")
ShareClass.create( stock_id:stock.id,
                   ticker:'BRK.A',
                   sclass:"A",
                   nshares: "811129",
                   votes:10000,
                   mul_factor:1 )
ShareClass.create( stock_id:stock.id,
                   ticker:'BRK.B',
                   sclass:"B",
                   nshares: "811129",
                   votes:1,
                   mul_factor:0.000666667 )


stock = Stock.find_by_ticker("GOOG")
ShareClass.create( stock_id:stock.id,
                   ticker:'GOOGL',
                   sclass:"A",
                   float_date: "2016-02-01",
                   nshares: "292580627",
                   votes:1,
                   mul_factor:1 )
ShareClass.create( stock_id:stock.id,
                   ticker:'-',
                   sclass:"B",
                   float_date: "2016-02-01",
                   nshares:"50199837",
                   votes:10,
                   mul_factor:1 )
ShareClass.create( stock_id:stock.id,
                   ticker:'GOOG',
                   sclass:"C",
                   float_date: "2016-02-01",
                   nshares:"345539303",
                   votes:0,
                   mul_factor:1 )

stock = Stock.find_by_ticker("AHC")
ShareClass.create( stock_id:stock.id,
                   ticker:'AHC',
                   sclass:"A",
                   votes:1,
                   mul_factor:1 )
ShareClass.create( stock_id:stock.id,
                   ticker:'-',
                   sclass:"B",
                   votes:10,
                   mul_factor:1 )

stock = Stock.find_by_ticker("NWSA")
stock = Stock.find_by_ticker("FOXA")
