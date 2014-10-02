# Methods added to this helper will be available to all templates in the application.
module ApplicationHelper

  def short_float(f)
    return 0 if f == 0 
    return "-" if f.nil? || f.to_f == 0.0 
    return "-" if f.nan?
    return "0" if f > 999999999999999999999999999999999999
    begin
      f2 = (f*100).to_i / 100.0
    rescue #above line can return Infinity
      f2 =  999.9
    end
    f2
  end
end
