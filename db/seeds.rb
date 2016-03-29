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

stock = Stock.find_by_ticker("CATO")
ShareClass.create( stock_id:stock.id,
                   ticker:'CATO',
                   sclass:"A",
                   votes:1,
                   nshares: "26129692",
                   float_date: "2016-01-30")
ShareClass.create( stock_id:stock.id,
                   ticker:'-',
                   sclass:"B",
                   votes:10,
                   nshares:"1743525",
                   float_date: "2016-01-30")
stock = Stock.find_by_ticker("CDI") #?

stock = Stock.find_by_ticker("CIA")
ShareClass.create( stock_id:stock.id,
                   ticker:'CIA',
                   sclass:"A",
                   votes:0,
                   nshares: "49080114",
                   float_date: "2016-03-07",
                   note: "Recieve twice the dividend of class B - BUT there is no div!")
ShareClass.create( stock_id:stock.id,
                   ticker:'-',
                   sclass:"B",
                   votes:1,
                   nshares: "1001714",
                   float_date: "2016-03-07",
                   note: "Vote on a board majority. All class B shares are owned by Harold E. Riley")

stock = Stock.find_by_ticker("CIX")
stock = Stock.find_by_ticker("D")
stock = Stock.find_by_ticker("DDI")
stock = Stock.find_by_ticker("DVD")
stock = Stock.find_by_ticker("SSP")
stock = Stock.find_by_ticker("EXL")
stock = Stock.find_by_ticker("FFG")
stock = Stock.find_by_ticker("AGM")
stock = Stock.find_by_ticker("FII")
stock = Stock.find_by_ticker("FGP")
stock = Stock.find_by_ticker("FXCM")
stock = Stock.find_by_ticker("GEN")
stock = Stock.find_by_ticker("GEF")
stock = Stock.find_by_ticker("HVT")
stock = Stock.find_by_ticker("HSY")
stock = Stock.find_by_ticker("H")
stock = Stock.find_by_ticker("IEP")
stock = Stock.find_by_ticker("IVC")
stock = Stock.find_by_ticker("LF")
stock = Stock.find_by_ticker("LEN")
stock = Stock.find_by_ticker("MCS")
stock = Stock.find_by_ticker("MWE")
stock = Stock.find_by_ticker("MSO")
stock = Stock.find_by_ticker("MKC")
stock = Stock.find_by_ticker("MOS")
stock = Stock.find_by_ticker("NC")
stock = Stock.find_by_ticker("NNI")
stock = Stock.find_by_ticker("NKE")
stock = Stock.find_by_ticker("NE")
stock = Stock.find_by_ticker("OB")
stock = Stock.find_by_ticker("OEH")
stock = Stock.find_by_ticker("PNM")
stock = Stock.find_by_ticker("PRU")
stock = Stock.find_by_ticker("SAI")
stock = Stock.find_by_ticker("SCG")
stock = Stock.find_by_ticker("SEMG")
stock = Stock.find_by_ticker("SPG")
stock = Stock.find_by_ticker("SKH")
stock = Stock.find_by_ticker("SPR")
stock = Stock.find_by_ticker("SR")
stock = Stock.find_by_ticker("SCS")
stock = Stock.find_by_ticker("TR")
stock = Stock.find_by_ticker("GTS")
stock = Stock.find_by_ticker("UNF")
stock = Stock.find_by_ticker("UAL")
stock = Stock.find_by_ticker("UPS")
stock = Stock.find_by_ticker("UBA")
stock = Stock.find_by_ticker("WSO")
stock = Stock.find_by_ticker("CUK")

stock = Stock.find_by_ticker("CMCSA")
ShareClass.create( stock_id:stock.id,
                   ticker:'CMCSA',
                   sclass:"A",
                   votes:1,
                   nshares: "2432953988",
                   float_date: "2015-12-31" )
ShareClass.create( stock_id:stock.id,
                   ticker:'-',
                   sclass:"B",
                   votes:129,
                   nshares: "9444375",
                   float_date: "2015-12-31",
                   note: "Class B, as a whole, has 1/3 the voting rights of combined classes A+B. Class B held by 3 people")
stock = Stock.find_by_ticker("DISCA")

stock = Stock.find_by_ticker("WIN")
