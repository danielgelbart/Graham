#!/usr/bin/ruby
  require 'rubygems'
  require 'fastercsv'

#This script copies the first two columns from the .csv file to the .txt file

output_file = File.open("sp500.txt","w")

FasterCSV.foreach("10-JUN-2009_500.csv") do |row|
  f1,f2,f3,f4,f5,f6 = row
  output_file.puts "#{f1}\t#{f2}"
end

output_file.close
