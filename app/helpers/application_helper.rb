# Methods added to this helper will be available to all templates in the application.
module ApplicationHelper
  Million = 1000000.0
  Thousand = 1000.0

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

  def is_to_s(stringint)
    intint = stringint.to_i / Million
    return nices(intint.to_s) + " Mil" if intint < 999
    nices(intint / Thousand) + " Bil"
  end

  def nices(num)
    number_with_precision( num.to_s, precision:2,
                           strip_insignificant_zeros:true, delimiter:',')
  end
end
