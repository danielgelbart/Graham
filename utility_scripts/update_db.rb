#!/usr/bin/ruby
require 'rubygems'
require 'date'

dd =  Date.today.month.to_i
puts "Running update #{dd}..."
cmd = "~/rails/Graham/cpp/bin/graham update_financials #{dd}"
ok = system( cmd )
puts "Ran updated. update returned "#{ok}"
