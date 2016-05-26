 #!/usr/bin/ruby
 require 'rubygems'
 require 'nokogiri'
 require 'open-uri'
 require 'date'

 def open_url_or_nil(url)
   begin
     doc = Nokogiri::HTML(open(url))
   rescue OpenURI::HTTPError => e
     nil
   else
     doc
   end
 end

 def write_to_file(file,doc)
   tab = doc.css("table").first
   tab_rows = tab.css("tr")
   tab_rows.shift #remove titls line

   tab_rows.each do |row|
     tds = row.css("td")
     file.puts "#{tds[0].text} , #{tds[1].text} , #{tds[7].text}"
     puts "Wrote to file: #{tds[0].text} , #{tds[1].text} , (cik):#{tds[7].text}"
   end

 end

 file = File.open('../sp500/' + "sp500_list_#{Date.today.to_s}.txt" ,"w")
 url = "https://en.wikipedia.org/wiki/List_of_S%26P_500_companies"
 doc = open_url_or_nil(url)
 write_to_file(file, doc) if doc

 file.close




