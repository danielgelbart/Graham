# Takes a stock ticker to get data for as an argument
namespace :stocks do
  desc "Delete non existant tickers"
  task :delete, [] => :environment do 
    require 'active_record'
    
 arr  = ["ABH", "AEI", "ACL", "AYE", "ALY", "ACRE", "ACF", "APO", "AHD", "AVO", "BEZ", "BVF", "BHS", "BF.A", "CBC", "CSFS", "CBF", "DCS", "CNB", "CTV", "CML", "CAL", "CYS", "DHRWI", "DLM", "DYP", "ECT", "FISWD", "FMR", "FRC", "FCEB", "FCEA", "GEOWI", "GLG", "GSL", "GTNA", "GAP", "GEFB", "HCA", "HEW", "HUBA", "HII", "IAR", "IMP", "VLT", "JCG", "JAS", "JWA", "KG", "KVA", "KVB", "LNY", "LEAWI", "LRP", "MX", "MSGY", "MFE", "MDRWI", "NNA", "NTY", "PLAA", "NAL", "PMG", "PLA", "Q", "RYI", "SEMG", "SVR", "TOD", "TRY", "UBP", "VRWD", "VZWI", "VOL", "JWB", "ZL"] 


  arr.each do |ticker| 
     s = Stock.find_by_ticker(ticker)
     if !s.nil?
       puts "Deleteing #{s.name}"
       s.delete
    end
  end
end
end