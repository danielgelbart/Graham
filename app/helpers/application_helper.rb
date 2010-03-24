# Methods added to this helper will be available to all templates in the application.
module ApplicationHelper

  def short_float(f)
    f2 = (f*100).to_i / 100.0
  end
end
