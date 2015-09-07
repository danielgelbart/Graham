namespace :scripts do

  class EDData
    attr_accessor :debt, :equity, :net_inc

    def initialize
      @debt = 0.0
      @equity = 0.0
      @net_inc = 0.0
    end

    def add_equity(val)
      @equity += val
      if @equity >= Float::MAX - 500
        puts "REACHEDDD MAAAXXX FLOAT VALUE"
      end

    end
  end

  desc "calculate earnings of s&p 500"
  task :earn => :environment do
    # open file
    file = File.open("./text_files/sp500_2015.txt","r")
    failed_tickers = []
    results = {}
    (2007..2014).each do |year|
      results[ year] = EDData.new
    end

    results[ "ttm" ] = EDData.new

    companies = []
    file.each_line do |line|
      companies << line.chomp
    end
    file.close
    companies << "BRKA"
    companies << "GOOG"

    results_file = File.new("sp500_earnings.txt","w+")
    # for each line do
    companies.each do |tikk|
      s = Stock.find_by_ticker(tikk)
      if s.nil?
        failed_tickers << tikk
        next
      end
      incs = s.annual_eps_newest_first.first(8)
      if incs.size < 8
        failed_tickers << tikk
        next
      end

      incs.each do |inc|
        next if results[inc.year].nil?
        results[ inc.year ].net_inc += num_to_bil( inc.net_income )
      end

      ttm_ear = s.ttm_earnings_record
      earnings = num_to_bil(ttm_ear.net_income) if !ttm_ear.nil?
      results["ttm"].net_inc += num_to_bil(ttm_ear.net_income) if !ttm_ear.nil?

      results_file.puts "#{tikk}: #{ earnings.to_s }"
    end

=begin    # print companies not included
    results_file = File.new("sp500_earnings.txt","w+")



    results_file.puts "Failed to get all data for #{failed_tickers.size} companies: "
    failed_tickers.each do |ticker|
      results_file.puts ticker
    end

    # print results
    results_file.puts "\n\nResults: "
    (2007..2014).each do |year|
      results_file.puts "YEAR: #{year}, EARNINGS: #{results[ year].net_inc}"
    end


    results_file.puts "TTM EARNINGS: #{results["ttm"].net_inc}"
=end
    results_file.close

  end

  desc "calculate debt of s&p 500"
  task :debt => :environment do
    # open file
    file = File.open("./text_files/sp500_2015.txt","r")
    failed_tickers = []
    results = {}
    (2007..2014).each do |year|
      results[ year] = EDData.new
    end

    companies = []
    file.each_line do |line|
      companies << line.chomp
    end
    file.close
    companies << "BRKA"
    companies << "GOOG"
    # for each line do
    companies.each do |tikk|
      s = Stock.find_by_ticker(tikk)
      if s.nil?
        failed_tickers << tikk
        next
      end
      bs = s.balance_sheets.sort_by(&:year).last(10)
      if bs.size < 10
        failed_tickers << tikk
        next
      end
      bs.each do |bs|
        next if results[ bs.year].nil?
        results[ bs.year ].debt += num_to_bil( bs.debt )
        results[ bs.year ].add_equity( num_to_bil(bs.equity) )
      end

    end
    results_file = File.new("equity_to_debt_results.txt","w+")
    # print companies not included
    results_file.puts "Failed to get all data for #{failed_tickers.size} companies: "
    failed_tickers.each do |ticker|
      results_file.puts ticker
    end

    # print results
    results_file.puts "\n\nResults: "
    (2007..2014).each do |year|
      results_file.puts "YEAR: #{year}, BOOK_VALUE: #{results[ year].equity}, DEBT: #{results[ year].debt}"
    end

    results_file.close
  end

  def num_to_bil(num)
    num.to_f / 1000000000
  end
end
