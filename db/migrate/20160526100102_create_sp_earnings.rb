class CreateSpEarnings < ActiveRecord::Migration
  def change
    create_table :sp_earnings do |t|
      t.date :calc_date
      t.string :list_file
      t.integer :num_included
      t.string :excluded_list
      t.string :total_market_cap
      t.string :public_market_cap
      t.string :total_earnings
      t.decimal :market_pe, precision:15, scale:2
      t.float :index_price
      t.decimal :inferred_divisor
      t.decimal :divisor_earnings, precision:15, scale:2
      t.decimal :divisor_pe, precision:15, scale:2
      t.string :notes

      t.timestamps
    end
  end
end
