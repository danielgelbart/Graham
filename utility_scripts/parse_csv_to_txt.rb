#!/usr/bin/ruby
  require 'rubygems'
  require 'fastercsv'

#This script copies the first two columns from the .csv file to the .txt file

output_file = File.open("sp500.txt","w")

FasterCSV.foreach("sp500-March-19-2010.csv") do |row|
  f1,f2,f3,f4,f5,f6 = row
  output_file.puts "#{f3}\t#{f2}"
end

output_file.close
