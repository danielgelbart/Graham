

class AddStocks < ActiveRecord::Migration
  def self.up
    file = File.open("nyse_stocks_list.txt","r")
    while (line = file.gets)
      ticker = line.split.first
      name = line.split.last
      s = Stock.create(:ticker => ticker, :name => name)
      puts "created #{s.ticker}" if s
    end
    file.close
  end

  def self.down
    raise ActiveRecord::IrreversibleMigration
  end
end
