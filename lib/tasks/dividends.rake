namespace :dividends do
  require 'selenium-webdriver'
  require 'nokogiri'
  desc "TODO"
  task :get_one, [:ticker] => :environment do |task, args|

    driver = Selenium::WebDriver.for :firefox
    driver.get "http://www.dividend.com"

    element = driver.find_element :name => "q"
    element.send_keys args[:ticker]
    element.submit

    begin
      element = driver.find_element(:id, "payout-history-table")
    rescue
      # we are returning (but from a rake task)
      puts "No dividends (or split) information found for #{args[:ticker]}"
      next;
    end

    stock = Stock.find_by_ticker(args[:ticker])
    html = Nokogiri::HTML(element.attribute('innerHTML'))

    parse_dividends(stock,html)
    stock.update_attributes( mark: "div-ok")

  end # end rake task

  task :get_all => :environment do

    ss = Stock.where( listed: true, mark: "0")

    # go to that page
    driver = Selenium::WebDriver.for :firefox
    driver.get "http://www.dividend.com"

    #puts "Page title is #{driver.title}"

    ss.each do |stock|
      sleep( rand(30) )

      if stock.id % 87 == 0
        driver = Selenium::WebDriver.for :firefox
        driver.get "http://www.dividend.com"
      end

      element = driver.find_element :name => "q"
      element.send_keys stock.ticker
      element.submit

      begin
        element = driver.find_element(:id, "payout-history-table")
      rescue
        # we are returning (but from a rake task)
        puts "No dividends (or split) information found for #{stock.ticker}"
        mark_stock_as_updated(stock)
        next;
      end
      html = Nokogiri::HTML(element.attribute('innerHTML'))
      parse_dividends(stock,html)

      mark_stock_as_updated(stock)
      end # loop over stocks
  end

  def parse_dividends(stock,html)

    rows = html.css('tr')
    rows.shift

    puts " All divs for #{stock.ticker} are:"
    rows.each do |row|

      amount = row.css('td')[0].text
      next if amount.empty?

      # Get Ex and PAY date of divs
      pay_date = row.css('td')[4].text
      ex_date = row.css('td')[2].text
      next if pay_date.empty? # for example STO newest div
      next if ex_date.empty? # for example STO newest div

      # handle as dividend
      if amount.to_s[0] == '$'
        amount[0] = ''
        div = Dividend.where( stock_id: stock.id, ex_date: ex_date.to_date).first
        if div.nil?
          create_new_dividend_record( stock, ex_date, pay_date, amount)
        else
          if div.ex_date == ex_date.to_date && div.pay_date.nil?
            div.update_attributes( pay_date: pay_date.to_date)
            puts "Updated existing div from ex date #{div.ex_date} with pay date #{div.pay_date}"
          end
        end
        # handle as stock split
      else
        split = Split.where( stock_id: stock.id, date: ex_date.to_date).first
        if split.nil?
          # 2-for-1 means 1 became 2
          if amount.scan(/\d+/).size != 2
            puts "Could not ge split data from string: #{amount}"
            next
          end
          split_to =  amount.scan(/\d+/)[0].to_i
          split_from =  amount.scan(/\d+/)[1].to_i

          spt = Split.create( stock_id: stock.id, date: ex_date.to_date,
                              base: split_from, into: split_to)
          puts "Added new stock split dated #{spt.date} | #{spt.base} becomes #{spt.into}"
        else
          puts "Already have split for #{stock.ticker} from #{ ex_date}"
        end

      end
    end
  end

  def create_new_dividend_record( stock, ex_date, pay_date, amount)
    div = Dividend.create( stock_id: stock.id, ex_date: ex_date.to_date,
                           pay_date: pay_date.to_date, amount: amount.to_f,
                           source: "dividend.com")
    puts "Added div for ex date #{ div.ex_date }, amount #{div.amount.to_f}"
    #div
  end

  def mark_stock_as_updated(stock)
    stock.update_attributes( mark: "div-ok")
  end

end
