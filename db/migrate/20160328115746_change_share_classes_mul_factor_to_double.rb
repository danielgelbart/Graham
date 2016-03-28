class ChangeShareClassesMulFactorToDouble < ActiveRecord::Migration
  def up
    change_column :share_classes, :mul_factor, :decimal, precision:20, scale:10, default:1.0
  end

  def down
    change_column :share_classes, :mul_factor, :integer, limit:3, default:1
  end
end
