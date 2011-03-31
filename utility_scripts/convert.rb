#!/usr/bin/ruby
require 'rubygems'
require 'fastercsv'

#This script copies the first two columns from the .csv file to the .txt file

output_file = File.open("products.csv","w")

def regenirate_upc(upc)
  upc
end

FasterCSV.foreach("test.csv") do |row|
  name, size_unit, price, image_tag, upc_partial = row
  size, unit = size_unit.split

  upc = regenirate_upc(upc_partial)
  #Do not registaer row if there is a field missing
  next if size.nil? || unit.nil? || upc.nil?
  output_file.puts "#{name}, #{size}, #{unit}, #{image_tag}, #{upc}"
end

output_file.close
