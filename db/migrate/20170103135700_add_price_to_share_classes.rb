class AddPriceToShareClasses < ActiveRecord::Migration
  def change
    add_column :share_classes, :latest_price, :decimal, :precision => 12, :scale => 6, :default => 0
  end
end
