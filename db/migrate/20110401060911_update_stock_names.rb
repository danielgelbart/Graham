class UpdateStockNames < ActiveRecord::Migration
  def self.up
    file = File.open("nyse_stocks_list.txt","r")
    while (line = file.gets)
      ticker = line.split.first
      s = Stock.find_by_ticker(ticker)
      s.update_attributes(:name => line.split("\t").last.chop) if s
      puts "updated #{s.ticker} name to #{line.split("\t").last.chop}" if s
    end
    file.close
  end

  def self.down
    raise ActiveRecord::IrreversibleMigration
  end
end
