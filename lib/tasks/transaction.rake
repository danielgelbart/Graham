namespace :transaction do
  desc "Load transactions from file"
  task :load => :environment do |task, args|
    require 'fastercsv'
    FasterCSV.foreach("data.csv") do |row|
      ticker,name,type,date,shares,price,commission,notes = row
      if Stock.find_by_ticker(ticker)

        Transaction.create(:type => type.upcase!,
                       :stock_id => Stock.find_by_ticker(ticker).id,
                       :date => Date.parse(date) || Date.new,
                       :comission => commission.to_i,
                       :shares => shares.to_i,
                       :price => price.to_f,
                       :portfolio_id => 1 )
      end
    end
  end
end
