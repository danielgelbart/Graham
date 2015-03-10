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

ActiveRecord::Schema.define(:version => 20150310082435) do

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
    t.integer  "quarter",             :default => 0
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
    t.decimal  "eps",         :precision => 12, :scale => 6
    t.integer  "stock_id"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.string   "revenue"
    t.string   "net_income"
    t.integer  "quarter",                                    :default => 0
    t.datetime "report_date"
    t.string   "shares"
  end

  create_table "notes", :force => true do |t|
    t.integer  "stock_id"
    t.integer  "year",        :limit => 2,                                                                                  :default => 2014
    t.enum     "pertains_to", :limit => [:K10, :Q10, :REPORTS, :INCOME_REP, :BALANCE_REP, :COVER_REP, :SHARES_OUTSTANDING], :default => :REPORTS
    t.string   "note"
    t.datetime "created_at",                                                                                                                      :null => false
    t.datetime "updated_at",                                                                                                                      :null => false
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
    t.integer  "market_cap"
  end

  create_table "stocks", :force => true do |t|
    t.string   "name"
    t.string   "ticker"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.decimal  "ttm_eps",                                                                                                                       :precision => 12, :scale => 6
    t.decimal  "book_value_per_share",                                                                                                          :precision => 12, :scale => 6
    t.integer  "dividends_per_year",                                                                                                                                           :default => 4
    t.decimal  "latest_price",                                                                                                                  :precision => 12, :scale => 6
    t.string   "market_cap"
    t.decimal  "ttm_div",                                                                                                                       :precision => 10, :scale => 3
    t.decimal  "yield",                                                                                                                         :precision => 6,  :scale => 3
    t.boolean  "listed",                                                                                                                                                       :default => true
    t.boolean  "has_currant_ratio",                                                                                                                                            :default => true
    t.string   "mark"
    t.integer  "cik"
    t.string   "fiscal_year_end",                                                                                                                                              :default => ""
    t.enum     "company_type",         :limit => [:COMPANY, :ROYALTY_TRUST, :REIT, :ASSET_MNGMT, :HOLDING, :INDUSTRY, :TECH, :PHARMA, :RETAIL],                                :default => :COMPANY
  end

  create_table "users", :force => true do |t|
    t.string   "name"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

end
