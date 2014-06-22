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



  desc "Get all statments for stock"
  task :get_statements, [:ticker] => :environment do |task, args|

#    ticker = "IBM"
    ticker = args[:ticker]
    stock = Stock.find_by_ticker(ticker)

    log = File.new(log_path("edgar_get_statements"),"w+")
    ed = Edgar.new(stock,log)

    ed.get_acns.list.each do |acn|
      # Only handle xbrl files, which only exist since 2009
      break if acn.year.to_i < 2009

      tenk = ed.get10k_text(acn)
      report_names = ed.find_names_of_income_and_balance_statements(tenk)
      ed.extract_and_save_income_and_balance_reports(report_names,acn.year)
    end

    log.close
  end


  desc "Get Income and Balance statements single year (specific) 10-k"
  task :get_single_year_statements => :environment do

    ticker = "IBM"
    stock = Stock.find_by_ticker(ticker)

    log = File.new(log_path("edgar_get_financials"),"w+")
    ed = Edgar.new(stock,log)

    tenks = ed.get_acns
    acn = tenks.first
    tenk = ed.get10k_text(acn)
    report_names = ed.find_names_of_income_and_balance_statements(tenk)
    ed.extract_and_save_income_and_balance_reports(report_names,acn.year)

    log.close
  end # task

  def log_path(name)
    dir = "financials"
    Dir.mkdir(dir) unless Dir.exists?(dir)
    log_path = File.join(dir,name+".log")
  end


end # namespace
