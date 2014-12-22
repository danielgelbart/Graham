namespace :copy do
  desc " Copy num shares data to ep model/table"
  task :nshares => :environment do |task, args|
    # Create stocks from a list
    ss = Stock.all

  #  s = Stock.find_by_ticker("CVX")
 #   a = Stock.find_by_ticker("BDX")
 #   ss = [s,a]
    ss.each do |s|
      s.numshares.each do |ns|
        ep = s.eps.select{ |e| e.year == ns.year }.first
        if ep.nil?
          puts "No ep for year #{ns.year} for #{s.ticker}"
          next
        end

        puts "Updating numshares for #{ s.ticker} to #{ns.shares} for year #{ns.year}"
        ep.shares = ns.shares
        ep.save!

      end
    end
  end
end
