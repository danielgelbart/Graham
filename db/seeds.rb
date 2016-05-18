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

stock = Stock.find_by_ticker("DDE")
ShareClass.create( stock_id:stock.id,
                   ticker:'-',
                   sclass:"A",
                   votes:10,
                   nshares: "14870673",
                   float_date: "2016-02-26")
ShareClass.create( stock_id:stock.id,
                   ticker:'DDE',
                   sclass:"B",
                   votes:1,
                   nshares:"18143942",
                   float_date: "2016-02-26")

stock = Stock.find_by_ticker("DVD")
ShareClass.create( stock_id:stock.id,
                   ticker:'-',
                   sclass:"A",
                   votes:10 )
ShareClass.create( stock_id:stock.id,
                   ticker:'DVD',
                   sclass:"",
                   votes:1 )

stock = Stock.find_by_ticker("SSP")
ShareClass.create( stock_id:stock.id,
                   ticker:'SSP',
                   sclass:"A",
                   votes:1,
                   note: "limited voting rights. Only elect 1/3 of the board")
ShareClass.create( stock_id:stock.id,
                   ticker:'-',
                   sclass:"",
                   votes:12 )

stock = Stock.find_by_ticker("FFG")
ShareClass.create( stock_id:stock.id,
                   ticker:'FFG',
                   sclass:"A",
                   votes:1 )
ShareClass.create( stock_id:stock.id,
                   ticker:'-',
                   sclass:"B",
                   votes:2,
                   note: "class B here is weird")

stock = Stock.find_by_ticker("AGM")
ShareClass.create( stock_id:stock.id,
                   ticker:'AGM.A',
                   sclass:"A",
                   votes:1,
                   note: "votes elect 1/3 of board")
ShareClass.create( stock_id:stock.id,
                   ticker:'-',
                   sclass:"B",
                   votes:2,
                   note: "elect a disjoint 1/3 of the board. The remainig 1/3 of board are appointed by the Presidend of the United States!!!")
ShareClass.create( stock_id:stock.id,
                   ticker:'AGM',
                   sclass:"C",
                   votes:0,
                   note: "This class has no vote. The remainig 1/3 of board are appointed by the Presidend of the United States!!!")

stock = Stock.find_by_ticker("FII")
sc = ShareClass.create( stock_id:stock.id,
                   ticker:'-',
                   sclass:"A",
                   votes:1,
                   note: "ALL voting rights, Helf by 1 entity (family)")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                   ticker:'FII',
                   sclass:"B",
                   votes:0,
                   note: "Very limited vote on extreme scenario. FFI is a 'controled' company")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("FXCM")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'FXCM',
                        sclass:"A",
                        votes:1)
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"B",
                        votes:1,
                        mul_factor:0,
                        note: "Do not share in the earnings. And have the following undeciferable voting writes: Class B common stock held by such holder, to one vote for each Holdings Unit in Holdings held by such holder.")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?


stock = Stock.find_by_ticker("GEN")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'GEN',
                        sclass:"A",
                        votes:1,
                        note: "")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"B",
                        votes:1,
                        note: "")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-(2)',
                        sclass:"C",
                        votes:1,
                        note: "This company has a bizzare stock class systme, along with highly suspicuous accounting policies")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?


stock = Stock.find_by_ticker("GEF")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'GEF',
                        sclass:"A",
                        mul_factor: 0.666,
                        nshares: "25693564",
                        float_date: "2015-12-16",
                        votes:0)
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'GEF.B',
                        sclass:"B",
                        votes:1,
                        nshares: "22119966",
                        float_date: "2015-12-16",
                        note: "divs, and claam on earnings are 1.5 times HIGHER for class B")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("HVT")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'HVT.A',
                        sclass:"A",
                        votes:10,
                        nshares: "2031349",
                        float_date: "2016-02-16",
                        note:"vote as a class to elect 75% of board")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'HVT',
                        sclass:"",
                        votes:1,
                        nshares: "20124844",
                        float_date: "2016-02-16",
                        note: "get divs tiny bit higher than class A")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("HSY")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'HSY',
                        sclass:"",
                        votes:1,
                        note:"Can elect, as a class, up to 1/6 of the board. Get divs 10% higher than class B")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"B",
                        votes:10)
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("H")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'H',
                        sclass:"A",
                        votes:1,
                        note:"All shareholders must vote in unicen with the chairman(!)")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"B",
                        votes:10)
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("IVC")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'IVC',
                        sclass:"",
                        votes:1,
                        nshares: "31752836",
                        float_date:"2016-05-04"
                        note:"")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"B",
                        nshares: "733309",
                        float_date:"2016-05-04"
                        votes:10,
                        note: "slightly reduced divs compared to other class")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

=begin ###Delisted!!!
stock = Stock.find_by_ticker("LF")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'LF',
                        sclass:"A",
                        votes:1,
                        note:"")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"B",
                        votes:10,
                        note: "")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
=end

stock = Stock.find_by_ticker("LEN")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'LEN',
                        sclass:"A",
                        votes:1,
                        nshares: "180111931",
                        float_date: "2015-12-31",
                        note:"")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'LEN.B',
                        sclass:"B",
                        votes:10,
                        nshares: "31303195",
                        float_date: "2015-12-31",
                        note: "Interestingly, class B trades for a LOWER price than class A")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("MCS")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'MCS',
                        sclass:"",
                        votes:1,
                        note:"")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"B",
                        votes:10,
                        note: "slightly reduced divs compared to other class")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("MKC")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'MKC.V',
                        sclass:"",
                        votes:1,
                        nshares: "11741812",
                        float_date: "2015-12-31",
                        note:"")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'MKC',
                        sclass:"non-voting",
                        votes:0,
                        nshares: "115366241",
                        float_date: "2015-12-31",
                        note: "")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

#No longer has share classes
stock = Stock.find_by_ticker("MOS")

stock = Stock.find_by_ticker("NC")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'NC',
                        sclass:"A",
                        nshares: "5307743",
                        float_date: "2016-04-29",
                        votes:1,
                        note:"")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"B",
                        nshares: "1571528",
                        float_date: "2016-04-29",
                        votes:10,
                        note: "")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("NNI")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'NNI',
                        sclass:"A",
                        votes:1,
                        note:"")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"B",
                        votes:10,
                        note: "Equal dividends given to share classes")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

#has a second share class, but not worth splitting
stock = Stock.find_by_ticker("NKE")

stock = Stock.find_by_ticker("OB")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'OB',
                        sclass:"A",
                        votes:1,
                        note:"")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"B",
                        votes:96,
                        note: "This class accounts for 96% of the voting power")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("BEL")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'BEL',
                        sclass:"A",
                        votes:1,
                        note:"")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"B",
                        votes:10,
                        note: "")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("SPG")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'SPG',
                        sclass:"A",
                        votes:0,
                        note:"")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"B",
                        votes:2,
                        note: "This class votes on directors. Same div as class A")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("GEN")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'GEN',
                        sclass:"A",
                        votes:1,
                        note: "")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"B",
                        votes:1,
                        note: "Voting is controled by non-marketable sub-group")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-(2)',
                        sclass:"C",
                        votes:1,
                        note: "Voting is controled by non-marketable sub-group")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("SPR")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'SPR',
                        sclass:"A",
                        votes:1,
                        note:"")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"B",
                        votes:1,
                        note: "Not clear what the nature of this class is")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("SCS")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'SCS',
                        sclass:"A",
                        votes:1,
                        note:"")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"B",
                        votes:10,
                        note: "Same div as class A")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("TR")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'TR',
                        sclass:"A"
                        float_date:"2016-03-31",
                        nshares:"38282506",
                        votes:1,
                        note:"")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"B",
                        votes:10,
                        note: "")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("GTS")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"A",
                        votes:1,
                        note:"")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'GTS',
                        sclass:"B",
                        votes:1,
                        note: "Not clear why the class A exist")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("UNF")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'UNF',
                        sclass:"A",
                        nshares:"15288535",
                        float_date:"2016-04-01",
                        votes:1,
                        note:"Gets a larger div than class B")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"B",
                        nshares:"4854519",
                        float_date:"2016-04-01",
                        votes:10,
                        note: "75% controlled by one family.")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("UPS")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'-',
                        sclass:"A",
                        votes:10,
                        note:"")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'UPS',
                        sclass:"B",
                        votes:1,
                        note: "")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("UBA")
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'UBA',
                        sclass:"A",
                        votes:1,
                        nshares: "26465544",
                        float_date: "2016-01-05",
                        note:"has 110% claim to div (and earnings) ovr common class")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?
sc = ShareClass.create( stock_id:stock.id,
                        ticker:'UBP',
                        sclass:"",
                        votes:20,
                        nshares: "9504378",
                        float_date: "2016-01-05",
                        note: "")
puts "#{stock.name}: Added SC #{sc.sclass}, ticker: #{sc.ticker} " if !sc.nil?

stock = Stock.find_by_ticker("WSO")
ShareClass.create( stock_id:stock.id,
                   ticker:'WSO',
                   sclass:"",
                   votes:1,
                   nshares: "30298038",
                   float_date: "2016-02-23")
ShareClass.create( stock_id:stock.id,
                   ticker:'WSOB',
                   sclass:"B",
                   votes:10,
                   nshares: "5035653",
                   float_date: "2016-02-23",
                   note: "divs equal between classes, class B elect 75% of board")



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
ShareClass.create( stock_id:stock.id,
                   ticker:'DISCA',
                   sclass:"A",
                   votes:1,
                   nshares: "150092266",
                   float_date: "2016-02-12" )
ShareClass.create( stock_id:stock.id,
                   ticker:'DISCB',
                   sclass:"B",
                   votes:10,
                   nshares: "6530284",
                   float_date: "2016-02-12",
                   note: "")
ShareClass.create( stock_id:stock.id,
                   ticker:'DISCK',
                   sclass:"C",
                   votes:0,
                   nshares: "253176880",
                   float_date: "2016-02-12" )

stock = Stock.find_by_ticker("BF.B")
ShareClass.create( stock_id:stock.id,
                   ticker:'BF.A',
                   sclass:"A",
                   votes:1,
                   nshares: "84528000",
                   float_date: "2015-06-30" )
ShareClass.create( stock_id:stock.id,
                   ticker:'BF.B',
                   sclass:"B",
                   votes:0,
                   nshares: "122483629",
                   float_date: "2015-06-30",
                   note: "")

stock = Stock.find_by_ticker("VIA")
ShareClass.create( stock_id:stock.id,
                   ticker:'VIA',
                   sclass:"A",
                   votes:1,
                   nshares: "49476685",
                   float_date: "2015-11-04" )
ShareClass.create( stock_id:stock.id,
                   ticker:'VIAB',
                   sclass:"B",
                   votes:0,
                   nshares: "347166576",
                   float_date: "2015-11-04",
                   note: "divs the same")

stock = Stock.find_by_ticker("REGN")
ShareClass.create( stock_id:stock.id,
                   ticker:'-',
                   sclass:"A",
                   nshares: "1913136",
                   float_date: "2016-04-14",
                   votes:10)
ShareClass.create( stock_id:stock.id,
                   ticker:'REGN',
                   sclass:"",
                   nshares: "103165457",
                   float_date: "2016-04-14",
                   votes:1,
                   note: "no divs for either class")



# There are two listings for CARNIVAL - CCL, and CUK
#stock = Stock.find_by_ticker("CUK")
