module SearchesHelper
  def sort_link_(column, title = nil)
    title ||= column.titleize
    direction = column == sort_column && sort_direction == "asc" ? "desc" : "asc"
    icon = sort_direction == "asc" ? "glyphicon glyphicon-chevron-up" : "glyphicon glyphicon-chevron-down"
    icon = column == sort_column ? icon : ""
    link_to "#{title} <span class='#{icon}'></span>".html_safe, {column: column, direction: direction}
  end

  def sort_link(title, sort_name, updown)
    if(@sort_col==sort_name)
      updown = @sort_dirc == "asc" ? "desc" : "asc"
    end
    icon = updown == "asc" ? "glyphicon glyphicon-chevron-up" : "glyphicon glyphicon-chevron-down"
    link_to "#{title} <span class='#{icon}'></span>".html_safe, {column: sort_name, direction: updown}
  end
end


