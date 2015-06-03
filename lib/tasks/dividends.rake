namespace :dividends do
  desc "TODO"
  task :get_one, [:ticker] => :environment do |task, args|
    require 'selenium-webdriver'
    # go to that page
    driver = Selenium::WebDriver.for :firefox
    driver.get "http://www.dividend.com"

    puts "Page title is #{driver.title}"

    element = driver.find_element :name => "q"
    element.send_keys "DE"
    element.submit

    puts "Page title is #{driver.title}"

    element = driver.find_element(:id, "payout-history-table")

    puts "#{element}"

    # endter ticker symbol in field

    # click get

    # get page

  end
  task :get => :environment do

  end

end
