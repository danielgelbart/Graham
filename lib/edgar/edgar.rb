# this class is retrieves data from edgar.gov
class Acn
  attr_accessor :ticker, :year, :acn, :cik

  def initialize(ticker,year,acn,cik)
    @ticker = ticker
    @year = year
    @acn = acn
    @cik = cik
  end

  def to_s
    "TICKER: "+ticker.to_s+" , YEAR: "+year.to_s+" , ACN: "+acn.to_s+"\n"
  end

  def url
    "http://www.sec.gov/Archives/edgar/data/#{cik}/#{acn}.txt"
  end
end


class AcnList
  attr_accessor :ticker, :name, :cik, :list

  def initialize(stock)
    @name = stock.name
    @cik = stock.cik
    @ticker = stock.ticker
    @list = []
  end

  def add(acn)
    @list << acn
  end

  def first
    @list.first
  end

  def to_s
    str = "NAME: "+name+" , TICKER: "+ticker+" , CIK: "+cik.to_s+"\n"
    @list.each do |rec|
      str += rec.to_s
    end
    str
  end
end

class Edgar

  attr_accessor :documents, :log, :stock

  def initialize(stock, log)
    self.stock = stock
    self.log = log
  end

  def get_acns
    ticker = stock.ticker
    cik = stock.cik

    if cik.nil?
      puts "cannot get accesions without cik for #{ticker}"
      return
    end

    puts "Getting ACN for #{ticker}"
    #out_file.puts("Name: "+ stock.name + " ; Ticker: "+ticker+" ; CIK: "+cik.to_s+"\n")

    url = "https://www.sec.gov/cgi-bin/browse-edgar?action=getcompany&CIK=#{cik}&type=10-k&dateb=&owner=exclude&count=40"

    doc = open_doc(url)
    return if doc.nil?

    table = doc.css('div#seriesDiv').xpath('.//tr')
    table.shift # get rid of first tr in table wich is headers


    acns = AcnList.new(stock)
    table.each do |tr|

      tds = tr.xpath('./td')

      #check that this row is for a 10-k
      next if tds[0].text != "10-K"

      # Capture the accistion number from the string
      acn = tds[2].text[/.*Acc-no: (?<acn>(\d|\-)*)/,"acn"]

      year = tds[3].text.first(4).to_i - 1
      year = year.to_s

      #At this point we have all the data needed to retriev 10 Ks
      # We have: cik and acn
      # We can either 1) Write this info to a file for latter use
      #               2) Get the file and save it for latter use
      #               3) Get the file, parse it and save the finantial data

      # Consider changing this method to end with
      # return ticker, cik, acn, year

      #        get_data(cik,acn,year)


      acns.add(Acn.new(ticker,year,acn,cik))
#      out_file.puts("YEAR "+ year +" ; ACN "+ acn +"\n")
#      puts "Wrote acn for #{year}"
    end

    acns
  end


  def get10k_text(acn)
    log.puts("edgar.rb get10k called with acn #{acn.to_s}")

    url = "http://www.sec.gov/Archives/edgar/data/#{acn.cik}/#{acn.acn}.txt"

    doc = open(url).read
    if doc.nil?
      log.puts("Could not get 10k for #{acn.ticker} #{acn.year} edgar.gov")
      return nil
    end

    doc
   # temporary open file for testing
#   File.open("IBM2013.txt","r").read
  end

  def find_names_of_income_and_balance_statements(doc)
    log.puts("Called find_names_of_income_and_balance_statements()")

    # Find the document in the text file that contains summary of all reports
    @documents = doc.split("<DOCUMENT>")

    found = false
    report_list = nil

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
      log.puts("Could not get report list for #{stock.ticker}")
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

    {:income => income_report_name ,
     :balance => balance_report_name }
  end


  def extract_and_save_income_and_balance_reports(names,year)
    if @documents.nil? || names.nil?
      log.puts("Nead to get 10k text file first")
      return
    end

    income_report_name = names[:income]
    balance_report_name = names[:balance]

    dir = "financials"

    stock_dir = ticker = stock.ticker
    stock_path = File.join(dir,stock_dir)
    Dir.mkdir(stock_path) unless Dir.exists?(stock_path)

    if !income_report_name.nil?
      found = false
      @documents.each do |document|
        document.each_line do |line|
          if line[/^<FILENAME>/]
            log.puts "found line #{line}"
            if !line.match(income_report_name).nil?
              file_path = File.join(stock_path,"#{ticker}_#{year}_income.txt")
              write_report_to_file(document,file_path)
              log.puts "created file #{file_path} containing file #{income_report_name}"
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
              write_report_to_file(document,file_path)
              log.puts "created file #{file_path} containing file #{balance_report_name}"
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
  end


  def get_income_data_from_statement(file)

    tab = get_table(file)

    units = get_units(tab)
    years = get_years(tab)

    numyears = years.size
    revs = get_revs(tab)
    incs = get_incs(tab)
    eps = get_dil_eps(tab)


    #    cal_dil_sharenum = incs / eps
    #    get_dil_sharenum

    (0..numyears-1).each do |i|
      x_ep = stock.ep_for_year(years[i])

      ep = Ep.create(:year => years[i].to_i,
                     :stock_id => stock.id,
                     :source => "edgar.gov",
                     :eps => eps[i].to_f,
                     :revenue => clean(revs[i],units),
                     :net_income => clean(incs[i],units))
      if ep.id.nil?
        puts "Could NOT create ep for #{stock.ticker} #{years[i]}"
        update_missing_data(x_ep,ep)
      else
        puts "SUCCESFULY created ep for #{stock.ticker} #{years[i]}"
      end
    end
  end

  private

  def clean(str,units)
    v = (str.gsub(",","").to_i * units).to_s
    puts v
    v
  end


  def get_table(file)
    doc = Nokogiri::HTML(file)
    tab = doc.css('//table').first if !doc.nil?
  end

  def get_years(tab)
    titels = tab.css('th')
    years = []
    titels.each do |th|
      begin
        date = th.text.to_date
        years << date.year
      rescue
        next
      end
    end
    years
  end

  def get_units(tab)
    titels = tab.css('th')
    unit = 1
    titels.each do |th|
      case th.text
      when /.*(in thou).*/i
        unit = 1000
        break
      when /.*(in mill).*/i
        unit = 1000000
        break
      when /.*(in bill).*/i
        unit = 1000000000
        break
      else
        next
      end
    end
    unit
  end

  def get_revs(tab)
    pattern = /(total revenue)/i
    num_pattern = /(\d+,?\d*,?\d*)/
    get_values_from_row(tab,pattern,num_pattern)
  end

  def get_incs(tab)
    pattern = /(net income)/i
    num_pattern = /(\d+,?\d*,?\d*)/
    get_values_from_row(tab,pattern,num_pattern)
  end

  def get_dil_eps(tab)
    pattern = /diluted|dilution/i
    per_share_pattern = /\$\s?(\d+\.\d\d)/
    get_values_from_row(tab,pattern,per_share_pattern)
  end

  def get_values_from_row(tab,pattern,num_pattern)
    vals = [""]
    tab.css('tr').each do |row|
      if !row.text[pattern].nil?
        vals = row.text.scan(num_pattern).flatten
        break
      end
    end
    if vals[0] == ""
      log.puts("ERROR: could not retriev total values from table")
    end

    puts "Values found for #{pattern.to_s} are:"
    vals.each do |v|
      puts v
    end
    vals
  end

  def differ(o,n)
    dif = (o - n).abs.to_f
    total = (o+n).to_f
    return (dif/total < 0.05)
  end

  def update_missing_data(o,n)
    if differ(o.revenue.to_i,n.revenue.to_i)
      log.puts("UPDATED revenue for #{o.year} from #{o.revenue} to #{n.revenue}")
      o.revenue = n.revenue
    end

    if differ(o.net_income.to_i,n.net_income.to_i)
      log.puts("UPDATED income for #{o.year} from #{o.net_income} to #{n.net_income}")
      o.net_income = n.net_income
    end

    if differ(o.eps.to_f,n.eps.to_f)
      log.puts("UPDATED eps for #{o.year} from #{o.eps} to #{n.eps}")
      o.eps = n.eps
    end
    o.save
  end



  def write_report_to_file(document,file_path)
    file = File.new(file_path,"w")

    # can write only table using get_table()

    file.puts("<DOCUMENT>\n") # this got choped off earliear
    file.puts(document)
    file.close
  end

  def open_doc(url)
    begin
      doc = Nokogiri::HTML(open(url))
    rescue OpenURI::HTTPError => e
      puts "Could not open url: #{e.message}"
      puts "For ticker #{ticker}"
      return
    end
  end

end # class


