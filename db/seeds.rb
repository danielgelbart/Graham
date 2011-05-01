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


#Stock.seed_many(:ticker,                [{:name => "Berkshire Hathaway", :ticker => "BRK"},                 {:name => "Teva Pharmasutical industries.", :ticker => "TEVA"}])

# Create stocks from a list
file = File.open("nyse_stocks_list.txt","r")
   while (line = file.gets)
     ticker = line.split.first
     name = line.split("\t").last.chop
     s = Stock.create(:ticker => ticker, :name => name)
     puts "created #{s.ticker}" if s
   end
file.close
