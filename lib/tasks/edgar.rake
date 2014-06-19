namespace :edgar do
  require 'active_record'
  require 'nokogiri'
  require 'open-uri'
  require 'edgar/edgar'


  desc "get data from edgar"
  task :get_acns => :environment do |task, args|

    out_file = File.new("accession_numbers.txt","w+")
    out_file.puts("This is a list of accession numbers for annual reprots on Edgar\n")

    stocks = Stock.all.select{ |s| s.cik != nil }

    stocks.each do |stock|
      ed = Edgar.new(stock)
      out_file.puts(ed.get_acns.to_s)
    end

    out_file.close
  end # task :get_acns


  desc "Get 10-k"
  task :get_10k

  end

  desc "Get Income and Balance statements from 10-k"
  task :get_financials => :environment do

    stock = Stock.find_by_ticker("IBM")

    ed = Edgar.new(stock,log)
    puts ed.stock
  end # task

end # namespace
