# Get edgar accesion numbers of annual reports on edgar
namespace :stock do
  desc "Get annual report, and its finantial data from edgar"

  task :edgar => :environment do |task, args|
    require 'active_record'
    require 'nokogiri'
    require 'open-uri'
    require 'fileutils'


    get_info
  end # task


  def get_info

    dir = "financials"
    Dir.mkdir(dir) unless Dir.exists?(dir)
    log_path = File.join(dir,"get_edgar_finantials.log")

    log = File.new(log_path,"w+")

    ticker = "CVX"
    stock = Stock.find_by_ticker(ticker)


    stock_dir = ticker.to_s
    stock_path = File.join(dir,stock_dir)
    Dir.mkdir(stock_path) unless Dir.exists?(stock_path)




    #    stocks = Stock.all.select{ |s| s.cik != nil }
    #    stocks.each do |stock|


    cik = stock.cik
    ticker = stock.ticker
    year = 2013.to_s

    # save download to disk
#    file = File.new("#{ticker}#{year}.txt","w")


    # IBM 2013
   # url = "http://www.sec.gov/Archives/edgar/data/51143/0001047469-14-001302.txt"
    # CVX 2013
    url = "http://www.sec.gov/Archives/edgar/data/93410/000009341014000024/0000093410-14-000024.txt"
    #Find the document in the text file that contains the summary of all reports
###    @documents = File.open("IBM2013.txt","r").read.split("<DOCUMENT>")
    @documents = open(url).read.split("<DOCUMENT>")


    found = false
    report_list = nil?
    @documents.reverse_each do |document|
      document.each_line do |line|
        if line[/^<FILENAME>/]
          log.puts "Found filename for document"
          if !line.match("FilingSummary.xml").nil?
            log.puts "Found the filing Summary!!!!!"
            report_list = Nokogiri::XML(document)
            found = true
          end
          break
        end #dealing with filename line
      end # lines
      break if found
    end # iterating over docs

    if report_list.nil?
      log.puts("Could not get report list for #{ticker}")
      log.close
      return
    end

    income_report_name = nil
    balance_report_name = nil
    #find wich reports are the income and balance statements
    report_list.search('//Report').each do |report|

      report_name = report.xpath('ShortName').text
      log.puts "checking a report #{report_name}"

      # skip if:
      next if !report_name[/.*\(Parenthetical\)/i].nil?

      if !report_name[/\s*consolidated statement of (earnings|income)\s*/i].nil?

        income_report_name = report.xpath('HtmlFileName').text
        log.puts "Found income report!!!!! its in the file: #{income_report_name}"
      end

      if !report_name[/\s*consolidated (statement of financial position|balance sheet)/i].nil?
        balance_report_name = report.xpath('HtmlFileName').text
        log.puts "Found Balance report!!!!! its in the file: #{balance_report_name}"
      end

      break if !income_report_name.nil? && !balance_report_name.nil?
    end

    # Save income report to a file using the name we got before
    if !income_report_name.nil?
      found = false
      @documents.each do |document|
        document.each_line do |line|
          if line[/^<FILENAME>/]
            log.puts "found line #{line}"
            if !line.match(income_report_name).nil?
              file_path = File.join(stock_path,"#{ticker}_#{year}_income.txt")
              log.puts "created file #{file_path} containing file #{income_report_name}"
              income_file = File.new(file_path,"w")
              income_file.puts(document)
              income_file.close
              found = true
            end
            break
          end
        end #line
        break if found
      end #documents
    else
      log.puts("Could not get income report for #{ticker} for #{year}")
    end


    #extract balance and incmoe report
    if !balance_report_name.nil?
      found = false
      @documents.each do |document|
        document.each_line do |line|
          if line[/^<FILENAME>/]
            log.puts "found line #{line}"
            if !line.match(balance_report_name).nil?
              file_path = File.join(stock_path,"#{ticker}_#{year}_balance.txt")
              log.puts "created file #{file_path} containing file #{balance_report_name}"
              balance_file = File.new(file_path,"w")
              balance_file.puts(document)
              balance_file.close
              found = true
            end
            break
          end
        end #line
        break if found
      end #documents
    else
      log.puts("Could not get balance report for #{ticker} for #{year}")
    end


    log.close
  end #get_info method


end #namespace :stock
