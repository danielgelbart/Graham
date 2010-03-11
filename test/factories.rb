 Factory.define :portfolio do |p|
   p.association :user
   p.sequence(:name)  {|n| "Portfolio_#{n}" }
 end

Factory.define :user do |u|
  u.sequence(:name) {|n| "Person_#{n}" }
end
