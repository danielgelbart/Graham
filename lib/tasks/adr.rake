# load ADR stock from .txt file

namespace :adr do
  desc " load ADR stock from .txt file"
  task :get_stocks => :environment do |task, args|
    # Create stocks from a list
    file = File.open("adr.txt","r")
    while (line = file.gets)
      if line.first != "-"
	name = line.split(";").first
	ticker = line.split(";").second.split.first

	s = Stock.find_by_ticker(ticker)
	if s.nil?
	  s = Stock.create(:ticker => ticker, :name => name)
	  puts "created #{s.ticker} for #{ s.name}" 
	else
 	  puts "stock for #{s.ticker} already exists" 
	end
      
      end # if line does not start with "-"
    end # while
    file.close

  end
end