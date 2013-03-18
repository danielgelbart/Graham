 module CommonDefs
  MILLION = 1000000
  BILLION = 1000000000
  YEAR = Time.new.year 

  def sum_to_i(num)
    case num
      when /\d+\.\d+ Bil/
        (num.to_f * BILLION).to_i
      when /\d+\.\d+ Mil/
        (num.to_f * MILLION).to_i
      else
        num.to_i
      end
  end



end
