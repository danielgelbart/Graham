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

    log = File.new(log_path,"w")

    ticker = "IBM"
    stock = Stock.find_by_ticker(ticker)


    stock_dir = ticker.to_s
    stock_path = File.join(dir,stock_dir)
    Dir.mkdir(stock_path) unless Dir.exists?(stock_path)




    #    stocks = Stock.all.select{ |s| s.cik != nil }
    #    stocks.each do |stock|


    cik = stock.cik
    ticker = stock.ticker
    year = 2013.to_s

#    file = File.new("#{ticker}#{year}.txt","w")

    # test url
    #url = "http://www.sec.gov/Archives/edgar/data/315189/000110465913090354/a13-17985_1ex12.htm"

    # real url
    url = "http://www.sec.gov/Archives/edgar/data/51143/0001047469-14-001302.txt"

    #Find the document in the text file that contains the summary of all reports
    documents = open(url).read.split("<DOCUMENT>")
    found = false
    report_list = nil?
    documents.reverse_each do |document|
      document.each_line do |line|
        if line[/^<FILENAME>/]
          puts "Found filename for document"
          if !line.match("FilingSummary.xml").nil?
            puts "Found the filing Summary!!!!!"
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

    #find wich reports are the income and balance statements
    report_list.search('//Report').each do |report|
      puts "checking a specific report"
      report_name = report.xpath('ShortName').text

      #TODO - For some reason this matching is not working

      if !report_name[/\s*consolidated statement of (earnings|income)\s*/i].nil?
        income_report_name = report.xpath('HtmlFileName').text
        puts "Found income report!!!!!"
      end
      if !report_name[/\s*consolidated (statement of financial position|balance sheet)\s*/i].nil?
        balance_report_name = report.xpath('HtmlFileName').text
      end
    end

    # Save income report to a file using the name we got before
    if defined? income_report_name
      found = false
      documents.each do |document|
        document.each_line do |line|
          if line[/^<FILENAME>/]
            puts "found line <Filename"
            if !line.match(income_report_name).nil?
              file_path = File.join(stock_path,"#{ticker}#{year}_income.txt")
              puts "created file #{file_path}"
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


    log.close
  end #get_info method


end #namespace :stock
