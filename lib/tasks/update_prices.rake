
namespace :prices do
  desc "Update prices for all stocks in DB"
  task :update => :environment do |task, args|

    require 'active_record'
    
    stocks = Stock.all
 
    stocks.map{ |s| s.update_price }
  end
end
