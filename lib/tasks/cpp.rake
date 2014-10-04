# Genertae cpp ORM files using dmm-metamaper
desc "generate DMMM C++ code"
task :mm => :environment do

  #require 'dm-core'
  require 'pry'

  require 'metamapper/metamapper'
  require 'metamapper/template'
  require 'metamapper/generator'
  require 'metamapper/generators/cpp'

  Rails.application.eager_load!

  models = ActiveRecord::Base.descendants
  models.sort!{ |a,b| a.name <=> b.name }

  MetaMapper.generate(:cpp,
                      :orm => :activerecord,
                      :models => models,
                      :output_dir =>
                        File.expand_path('../../../cpp/src/dmmm/', __FILE__))
end

