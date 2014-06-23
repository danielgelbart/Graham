namespace :edgar do
  require 'active_record'
  require 'nokogiri'
  require 'open-uri'
  require 'edgar/edgar'


  desc "get accestion numbers of 10-ks frmo edgar"
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



  desc "Get all available xbrl statments for single stock"
  task :get_statements, [:ticker] => :environment do |task, args|

#    ticker = "IBM"
    ticker = args[:ticker]
    stock = Stock.find_by_ticker(ticker)

    log = File.new(log_path("edgar_get_statements_#{ticker}",ticker),"w+")

    ed = Edgar.new(stock,log)

    # Only getting reports for:
    # - 2013 Most recent
    # - 2011 (overlaps for 2011 with 2013 report)
    # - 2009 The FIRST available xbrl report (overlaps for 2009 with 2011 report
    # These are the only 3 reports returned by AcnList.xbrls
    ed.get_acns.xbrls.each do |acn|
      tenk = ed.get10k_text(acn)
      report_names = ed.find_names_of_income_and_balance_statements(tenk)
      ed.extract_and_save_data(report_names,acn.year)
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

  desc "Testing the parsing of financial data"
  task :extract_data => :environment do

    ticker = "IBM"
    stock = Stock.find_by_ticker(ticker)

    log = File.new(log_path("edgar_get_financials"),"w+")
    ed = Edgar.new(stock,log)

    doc = open("financials/IBM/IBM_2013_income.txt").read
    ed.get_income_data_from_statement(doc)

    log.close

  end

  def log_path(name, stock_dir = nil)
    dir = "financials"
    Dir.mkdir(dir) unless Dir.exists?(dir)

    if !stock_dir.nil?
      dir = File.join(dir,stock_dir)
      Dir.mkdir(dir) unless Dir.exists?(dir)
    end
    log_path = File.join(dir,name+".log")
  end


end # namespace
