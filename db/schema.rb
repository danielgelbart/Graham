# This file is auto-generated from the current state of the database. Instead of editing this file, 
# please use the migrations feature of Active Record to incrementally modify your database, and
# then regenerate this schema definition.
#
# Note that this schema.rb definition is the authoritative source for your database schema. If you need
# to create the application database on another system, you should be using db:schema:load, not running
# all the migrations from scratch. The latter is a flawed and unsustainable approach (the more migrations
# you'll amass, the slower it'll run and the greater likelihood for issues).
#
# It's strongly recommended to check this file into your version control system.

ActiveRecord::Schema.define(:version => 20120711172842) do

  create_table "balance_sheets", :force => true do |t|
    t.integer  "stock_id"
    t.integer  "year"
    t.string   "current_assets"
    t.string   "total_assets"
    t.string   "current_liabilities"
    t.string   "total_liabilities"
    t.string   "long_term_debt"
    t.string   "book_value"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.string   "net_tangible_assets"
    t.string   "total_sales"
  end

  create_table "dividends", :force => true do |t|
    t.integer  "stock_id"
    t.date     "date"
    t.decimal  "amount",     :precision => 12, :scale => 8
    t.string   "source"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "eps", :force => true do |t|
    t.integer  "year"
    t.string   "source"
    t.decimal  "eps",        :precision => 12, :scale => 6
    t.integer  "stock_id"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "numshares", :force => true do |t|
    t.integer  "stock_id"
    t.integer  "year"
    t.string   "shares"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "owned_stocks", :force => true do |t|
    t.integer  "portfolio_id"
    t.integer  "stock_id"
    t.integer  "shares"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "portfolios", :force => true do |t|
    t.integer  "user_id"
    t.string   "name"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "splits", :force => true do |t|
    t.integer  "stock_id"
    t.integer  "old"
    t.integer  "new"
    t.decimal  "split_mul_factor",         :precision => 10, :scale => 2
    t.date     "date"
    t.boolean  "stock_adjusted_for_split",                                :default => false
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "stocks", :force => true do |t|
    t.string   "name"
    t.string   "ticker"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.decimal  "ttm_eps",                :precision => 12, :scale => 6
    t.decimal  "book_value_per_share",   :precision => 12, :scale => 6
    t.date     "finantial_data_updated"
    t.integer  "dividends_per_year",                                    :default => 4
    t.decimal  "latest_price",           :precision => 12, :scale => 6
    t.string   "market_cap"
    t.decimal  "ttm_div",                :precision => 10, :scale => 3
    t.decimal  "yield",                  :precision => 6,  :scale => 3
    t.boolean  "listed",                                                :default => true
    t.boolean  "has_currant_ratio",                                     :default => true
  end

  create_table "transactions", :force => true do |t|
    t.date     "date"
    t.decimal  "comission",    :precision => 10, :scale => 2
    t.integer  "stock_id"
    t.integer  "shares"
    t.boolean  "type"
    t.decimal  "price",        :precision => 10, :scale => 2
    t.integer  "portfolio_id"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "users", :force => true do |t|
    t.string   "name"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

end
