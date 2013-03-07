# Methods added to this helper will be available to all templates in the application.
module ApplicationHelper

  def short_float(f)
    return 0 if f == 0
    return "-" if f.nil? 
    return "-" if f.nan?
    return "0" if f > 999999999999999999999999999999999999
    f2 = (f*100).to_i / 100.0
  end
end
