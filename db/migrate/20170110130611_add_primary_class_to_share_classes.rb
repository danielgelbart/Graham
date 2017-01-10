class AddPrimaryClassToShareClasses < ActiveRecord::Migration
  def change
    add_column :share_classes, :primary_class, :boolean, default:false
  end
end
