 #!/usr/bin/ruby
 require 'rubygems'
 require 'nokogiri'
 require 'open-uri'


 #require 'date'



ss = Stock.all

ss.each do |s|
   s.splits.each do |sp|

     dups = s.splits.select{ |p| p.date == sp.date }
     dups.each do |d|
       d.destroy if (d.id != sp.id)
     end
   end
end
