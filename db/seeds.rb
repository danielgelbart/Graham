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
ShareClass.create( stock_id:stock.id,
                   ticker:'NWSA',
                   sclass:"A",
                   votes:0,
                   nshares: "380790614",
                   float_date: "2016-01-16" )
ShareClass.create( stock_id:stock.id,
                   ticker:'NWS',
                   sclass:"B",
                   votes:1,
                   nshares: "199630240",
                   float_date: "2016-01-16")

stock = Stock.find_by_ticker("FOXA")
ShareClass.create( stock_id:stock.id,
                   ticker:'FOXA',
                   sclass:"A",
                   votes:0,
                   nshares: "1220939959",
                   float_date: "2015-08-07")
ShareClass.create( stock_id:stock.id,
                   ticker:'FOX',
                   sclass:"B",
                   votes:1,
                   nshares: "798520953",
                   float_date: "2015-08-07")

stock = Stock.find_by_ticker("AOS")
ShareClass.create( stock_id:stock.id,
                   ticker:'-',
                   sclass:"A",
                   votes:10,
                   nshares: "13121508",
                   float_date: "2016-02-10")
ShareClass.create( stock_id:stock.id,
                   ticker:'AOS',
                   sclass:"B",
                   votes:1,
                   nshares: "74677900",
                   float_date: "2016-02-10",
                   note: "Class B share (the public float) are limited as a class to elect 1/3 of the board")

stock = Stock.find_by_ticker("SAM")
ShareClass.create( stock_id:stock.id,
                   ticker:'SAM',
                   sclass:"A",
                   votes:0,
                   nshares: "9465306",
                   float_date: "2016-02-12",
                   note: "Class A holders can only elect a minority of board members")
ShareClass.create( stock_id:stock.id,
                   ticker:'-',
                   sclass:"B",
                   votes:1,
                   nshares: "3367355",
                   float_date: "2016-02-12",
                   note: "All class B shares are owned by the infamous Mr. C. James Koch")


stock = Stock.find_by_ticker("BAH")
ShareClass.create( stock_id:stock.id,
                   ticker:'BAH',
                   sclass:"A",
                   votes:1)
ShareClass.create( stock_id:stock.id,
                   ticker:'-',
                   sclass:"B",
                   votes:0,
                   note: "convertable to class A")
ShareClass.create( stock_id:stock.id,
                   ticker:'-(1)',
                   sclass:"C",
                   votes:1,
                   note: "convertable to class A")
ShareClass.create( stock_id:stock.id,
                   ticker:'-(2)',
                   sclass:"E",
                   votes:1,
                   note: "convertable to class A-Derived from options")

stock = Stock.find_by_ticker("CCL")
stock = Stock.find_by_ticker("CATO")
stock = Stock.find_by_ticker("CDI")
stock = Stock.find_by_ticker("ACMP")
stock = Stock.find_by_ticker("CIA")
stock = Stock.find_by_ticker("CMS")
