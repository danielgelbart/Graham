module StocksHelper

  def add_data_link(form_builder)
    link_to_function 'Add data for another year', nil, do |page|
      form_builder.fields_for :balance_sheet, BalanceSheet.new, :child_index => 'NEW_RECORD' do |f|
        html = render(:partial => 'balance_sheet', :locals => { :ad_form => f })
        page << "$('finantials').insert({ bottom: '#{escape_javascript(html)}'.replace(/NEW_RECORD/g, new Date().getTime()) });"

      end
    end
  end

  def remove_data_link(form_builder)
    if form_builder.object.new_record?
      # If this is a new record, we can just remove the div from the dom
      link_to_function("remove", "$(this).up('.balance_sheet').remove();");
    else
      # However if it's a "real" record it has to be deleted from the database,
      # for this reason the new fields_for, accept_nested_attributes helpers give us _delete,
      # a virtual attribute that tells rails to delete the child record.
      ## form_builder.hidden_field(:delete) +
      ## link_to_function("remove", "$(this).up('.balance_sheet').hide(); $(this).previous().value = '1'")
    end
  end

  def add_earnings_link(form_builder)
    link_to_function 'Add Earnings data for another year', nil, do |page|
      form_builder.fields_for :ep, Ep.new, :child_index => 'NEW_RECORD' do |f|
        html = render(:partial => 'ep', :locals => { :e_form => f })
        page << "$('eps').insert({ bottom: '#{escape_javascript(html)}'.replace(/NEW_RECORD/g, new Date().getTime()) });"

      end
    end
  end

  def remove_earnings_link(form_builder)
    if form_builder.object.new_record?
      # If this is a new record, we can just remove the div from the dom
      link_to_function("remove", "$(this).up('.ep').remove();");
    else
      # However if it's a "real" record it has to be deleted from the database,
      # for this reason the new fields_for, accept_nested_attributes helpers give us _delete,
      # a virtual attribute that tells rails to delete the child record.
      ## form_builder.hidden_field(:delete) +
      ## link_to_function("remove", "$(this).up('.ep').hide(); $(this).previous().value = '1'")
    end
  end

  def yes_no(value)
    value == false ? "No" : "Yes"
  end

end
