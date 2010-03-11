# Some additional config info for cucumber, In case env.rb gets overwriten

# Use factories to create test models
require File.join(RAILS_ROOT, 'test', 'factories')

Cucumber::Rails::World.class_eval do
  include ActionView::Helpers::RecordIdentificationHelper
end

# Seed data for tests
Dir[File.join(RAILS_ROOT, 'db', 'seeds.rb')].each do |file|
  load file
end

# Use rsped to stub models
require "spec/mocks"

Before do
  $rspec_mocks ||= Spec::Mocks::Space.new
end

After do
  begin
    $rspec_mocks.verify_all
  ensure
    $rspec_mocks.reset_all
  end
end
