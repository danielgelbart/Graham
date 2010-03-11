Given /^I have a portfolio called "([^\"]*)"$/ do |name|
  @user.portfolios << Factory(:portfolio, :user_id => @user.id, :name => name)
end

When /^I select a date of purchase$/ do
#Do nothing
end

Then /^I should see "([^\"]*)" and "([^\"]*)" displayed$/ do |shares, ticker|
  stock = OwnedStock.find(:first, :conditions => { :portfolio_id => 1, :stock_id => Stock.find_by_ticker(ticker).id }) || raise("A stock with ticker #{ticker} was not found in the portfolio.")
dom_id_name = dom_id(stock)
  Then %|I should see "#{ticker}" within "##{dom_id_name}"|
  Then %|I should see "#{shares}" within "##{dom_id_name}"|
end

Then /^I should own "([^\"]*)" shares of "([^\"]*)"$/ do |shares,ticker|
  o = OwnedStock.find(:first, :conditions => { :stock_id => Stock.find_by_ticker(ticker).id, :portfolio_id => 1})
  o.shares.should == shares.to_i
end
