namespace :export do
  desc "Prints Stocks.all to create a seed file for seeds.rb "
  task :seed_stocks => :environment do

    out_file = File.new("stock_seeds.rb","w+")

    Stock.order(:id).all.each do |s|
      next if s.cik.nil?
      out_file.puts "Stock.create( 'name'=>'#{ s.name.gsub(/\'/,' ') }','ticker'=>'#{s.ticker}','cik'=>#{s.cik},'fyed'=>'#{s.fiscal_year_end}')"
    end
    out_file.close

  end #task
end #namespace
