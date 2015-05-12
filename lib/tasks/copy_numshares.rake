namespace :copy do
  desc " Copy num shares data to ep model/table"
  task :nshares => :environment do |task, args|
    # Create stocks from a list
 ss = Stock.all

#   s = Stock.find_by_ticker("CVX")
 #   ss = [s,a]
    ss.each do |s|
    s.numshares.each do |ns|
      ep = s.eps.select{ |e| e.year == ns.year && e.quarter == 0 }.first
      if ep.nil?
        puts "No ep for year #{ns.year} for #{s.ticker}"
        next
      end

      str = ns.shares
      if str.last(3) == "Bil"
        num = str.to_f * 1000000000
      else
        if str.last(3) == "Mil"
          num = str.to_f * 1000000
        else
          num = str.to_i
        end
      end

      num = num.to_i.to_s

      puts "Changing numshares for #{s.ticker} from #{ns.shares} to #{num} for year #{ns.year}"
      ep.shares = num
      ep.save!

    end #numshares
   end #stocks
  end #task
#  end
end #namespace

