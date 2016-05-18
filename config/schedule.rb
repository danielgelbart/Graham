# Use this file to easily define all of your cron jobs.
#
# It's helpful, but not entirely necessary to understand cron before proceeding.
# http://en.wikipedia.org/wiki/Cron

# Example:
#
# set :output, "/path/to/my/cron_log.log"
#
# every 2.hours do
#   command "/usr/bin/some_great_command"
#   runner "MyModel.some_method"
#   rake "some:great:rake:task"
# end
#
# every 4.days do
#   runner "AnotherModel.prune_old_records"
# end

# Learn more: http://github.com/javan/whenever
set :output, "../log/cron_whenever.log"

every 1.months do
  # General Update
  # update financials
  command "../cpp/bin/graham update_financials #{Date.today.month}"
  # update div/splits
  # update price
#  runner Stock.update_prices

  # S&P 500 data update
  # Get latest list
  # Get index price
  # Run sppe calculations, and save to disk
end
