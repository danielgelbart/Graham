# encoding: UTF-8
# This file is auto-generated from the current state of the database. Instead
# of editing this file, please use the migrations feature of Active Record to
# incrementally modify your database, and then regenerate this schema definition.
#
# Note that this schema.rb definition is the authoritative source for your
# database schema. If you need to create the application database on another
# system, you should be using db:schema:load, not running all the migrations
# from scratch. The latter is a flawed and unsustainable approach (the more migrations
# you'll amass, the slower it'll run and the greater likelihood for issues).
#
# It's strongly recommended to check this file into your version control system.

ActiveRecord::Schema.define(:version => 20170103135700) do

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
    t.integer  "quarter",             :default => 0
    t.boolean  "calculated_bv"
    t.boolean  "calculated_tl"
  end

  create_table "dividends", :force => true do |t|
    t.integer  "stock_id"
    t.date     "ex_date"
    t.decimal  "amount",     :precision => 12, :scale => 8
    t.string   "source"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.date     "pay_date"
  end

  add_index "dividends", ["ex_date"], :name => "index_dividends_on_ex_date"
  add_index "dividends", ["pay_date"], :name => "index_dividends_on_pay_date"
  add_index "dividends", ["stock_id"], :name => "index_dividends_on_stock_id"

  create_table "eps", :force => true do |t|
    t.integer  "year"
    t.string   "source"
    t.decimal  "eps",            :precision => 12, :scale => 6
    t.integer  "stock_id"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.string   "revenue"
    t.string   "net_income"
    t.integer  "quarter",                                       :default => 0
    t.datetime "report_date"
    t.string   "shares"
    t.boolean  "shares_diluted",                                :default => true
  end

  create_table "notes", :force => true do |t|
    t.integer  "stock_id"
    t.integer  "year",        :limit => 2,                                                                                                                         :default => 2014
    t.enum     "pertains_to", :limit => [:K10, :Q10, :REPORTS, :INCOME_REP, :NO_REV, :NO_INC, :NO_SHARES, :NO_EPS, :BALANCE_REP, :COVER_REP, :SHARES_OUTSTANDING], :default => :REPORTS
    t.string   "note"
    t.datetime "created_at",                                                                                                                                                             :null => false
    t.datetime "updated_at",                                                                                                                                                             :null => false
  end

  create_table "numshares", :force => true do |t|
    t.integer  "stock_id"
    t.integer  "year"
    t.string   "shares"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "searches", :force => true do |t|
    t.boolean  "no_losses"
    t.boolean  "divs"
    t.float    "dilution"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.boolean  "good_balance"
    t.integer  "book"
    t.boolean  "current_div"
    t.boolean  "earning_growth"
    t.boolean  "defensive_price"
    t.boolean  "big_enough"
    t.string   "market_cap"
    t.string   "sort_by"
  end

  create_table "share_classes", :force => true do |t|
    t.integer  "stock_id"
    t.string   "ticker",       :limit => 8
    t.string   "sclass",       :limit => 3
    t.integer  "votes"
    t.string   "nshares"
    t.date     "float_date"
    t.decimal  "mul_factor",                :precision => 20, :scale => 10, :default => 1.0
    t.string   "note"
    t.datetime "created_at",                                                                 :null => false
    t.datetime "updated_at",                                                                 :null => false
    t.decimal  "latest_price",              :precision => 12, :scale => 6,  :default => 0.0
  end

  add_index "share_classes", ["sclass"], :name => "index_share_classes_on_sclass"
  add_index "share_classes", ["stock_id"], :name => "index_share_classes_on_stock_id"
  add_index "share_classes", ["ticker"], :name => "index_share_classes_on_ticker"

  create_table "sp_earnings", :force => true do |t|
    t.date     "calc_date"
    t.string   "list_file"
    t.integer  "num_included"
    t.string   "excluded_list"
    t.string   "total_market_cap"
    t.string   "public_market_cap"
    t.string   "total_earnings"
    t.decimal  "market_pe",         :precision => 15, :scale => 2
    t.float    "index_price"
    t.decimal  "inferred_divisor",  :precision => 10, :scale => 0
    t.decimal  "divisor_earnings",  :precision => 15, :scale => 2
    t.decimal  "divisor_pe",        :precision => 15, :scale => 2
    t.string   "notes"
    t.datetime "created_at",                                       :null => false
    t.datetime "updated_at",                                       :null => false
  end

  create_table "splits", :force => true do |t|
    t.integer  "stock_id"
    t.date     "date"
    t.integer  "base"
    t.integer  "into"
    t.datetime "created_at", :null => false
    t.datetime "updated_at", :null => false
  end

  add_index "splits", ["date"], :name => "index_splits_on_date"
  add_index "splits", ["stock_id"], :name => "index_splits_on_stock_id"

  create_table "stocks", :force => true do |t|
    t.string   "name"
    t.string   "ticker"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.integer  "dividends_per_year",                                                                                                                                                                                      :default => 4
    t.decimal  "latest_price",                                                                                                                                                             :precision => 12, :scale => 6
    t.decimal  "ttm_div",                                                                                                                                                                  :precision => 10, :scale => 3
    t.decimal  "yield",                                                                                                                                                                    :precision => 6,  :scale => 3
    t.boolean  "listed",                                                                                                                                                                                                  :default => true
    t.boolean  "has_currant_ratio",                                                                                                                                                                                       :default => true
    t.string   "mark"
    t.integer  "cik"
    t.string   "fiscal_year_end",                                                                                                                                                                                         :default => ""
    t.enum     "company_type",       :limit => [:COMPANY, :ROYALTY_TRUST, :REIT, :ASSET_MNGMT, :FINANCE, :PARTNERSHIP, :PIPELINE, :FOREIGN, :HOLDING, :INDUSTRY, :TECH, :PHARMA, :RETAIL],                                :default => :COMPANY
    t.string   "country"
    t.boolean  "fy_same_as_ed",                                                                                                                                                                                           :default => true
  end

  create_table "users", :force => true do |t|
    t.string   "name"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

end
