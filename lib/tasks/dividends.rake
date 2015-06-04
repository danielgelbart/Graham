namespace :dividends do
  desc "TODO"
  task :get_one, [:ticker] => :environment do |task, args|
    require 'selenium-webdriver'
    require 'nokogiri'

    # go to that page
    driver = Selenium::WebDriver.for :firefox
    driver.get "http://www.dividend.com"

    #puts "Page title is #{driver.title}"

    element = driver.find_element :name => "q"
    element.send_keys args[:ticker]
    element.submit

    #puts "Page title is #{driver.title}"

    begin
      element = driver.find_element(:id, "payout-history-table")
    rescue
      # we are returning (but from a rake task)
      puts "No dividends (or split) information found for #{args[:ticker]}"
      next;
    end

    stock = Stock.find_by_ticker(args[:ticker])

    html = Nokogiri::HTML(element.attribute('innerHTML'))
    rows = html.css('tr')
    rows.shift

    # what does a stock that stopped paying divs look like?
    # has no declared up comming dividend!

    puts " All divs for #{args[:ticker]} are:"
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
          puts "Already have split for #{args[:ticker]} from #{ ex_date}"
        end

      end
    end

  end # end rake task

  def create_new_dividend_record( stock, ex_date, pay_date, amount)
    div = Dividend.create( stock_id: stock.id, ex_date: ex_date.to_date,
                           pay_date: pay_date.to_date, amount: amount.to_f,
                           source: "dividend.com")
    puts "Added div for ex date #{ div.ex_date }, amount #{div.amount.to_f}"
    #div
  end


  task :get => :environment do

  end

end
