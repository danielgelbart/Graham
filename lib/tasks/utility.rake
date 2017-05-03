# For writing various utility scripts
namespace :utility do
  desc "Add forth quarter report dates"
  task :sync_report_dates => :environment do
    ss = Stock.where( listed: true)

    # NO GOOD! report_date in DB is date of quarter end!
    # NOT the date the report was published

    ss.each do |s|
      s.quarters.select{ |q| q.quarter == 4}.each do |fq|
        annual = s.ep_for_year(fq.year)
        fq.update_attributes( report_date: annual.report_date)
        puts "Updated rp date for #{s.ticker} #{fq.year} to #{annual.report_date}"
      end
    end

  end



end
