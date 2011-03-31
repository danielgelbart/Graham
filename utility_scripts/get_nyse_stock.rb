 #!/usr/bin/ruby
 require 'rubygems'
 require 'nokogiri'
 require 'open-uri'


 def open_url_or_nil(url)
   begin
     doc = Nokogiri::HTML(open(url))
   rescue OpenURI::HTTPError => e
     nil
   else
     doc
   end
 end

# this is a template url for NYSE website
 def get_page(num)
   url = "http://www.nyse.com/about/listed/lc_ny_issuetype_1076458359969.html?ListedComp=All&start=1601&item=#{num}&startlist=1&group=9&firsttime=done"
 end


 def save_to_file(file,doc)
   try = doc.xpath('//td/table')
   tt = try.xpath('//table/tr')
   tab = tt[5]
   tabc = tab.children.first
   qw = tabc.children[1]
   as =qw.children[6]
   asd = as.children.first
   list = asd.text.split("[")
   (14..1745).each do |i|
     file.puts "#{ list[i].split("\"")[1] }\t#{ list[i].split("\"")[3] }"
     puts "Wrote to file #{ list[i].split("\"")[1] }\t#{ list[i].split("\"")[3] }"
   end

 end

 file = File.open("nyse_stocks_list.txt","w")

 doc = open_url_or_nil(get_page(1))
 save_to_file(file, doc) if doc

file.close




