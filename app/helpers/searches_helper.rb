module SearchesHelper

  def sort_link(title, sort_name, updown)
    if(@sort_col==sort_name)
      updown = @sort_dirc == "asc" ? "desc" : "asc"
    end
    icon = updown == "desc" ? "glyphicon glyphicon-chevron-up" : "glyphicon glyphicon-chevron-down"
    link_to "#{title} <span class='#{icon}'></span>".html_safe, {column: sort_name, direction: updown}
  end
end


