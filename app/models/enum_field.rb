module EnumField

  def enum_fields
    @enum_fields ||= []
  end

  def enum_field?(name)
    enum_fields.member?(name)
  end

  def enum_field(property, opts={})
    enum_fields << property unless enum_fields.member?(property)
    dictionary = opts[:options].map.with_index {|opt, i|
      OpenStruct.new name: opt, value: i + 1
    }
    default = opts[:default]

    define_method property do
      val = self[property]
      option = dictionary.find {|opt| opt.value == val}
      option.try(:name)
    end

    define_method :"#{property}=" do |value|
      option = dictionary.find {|opt| opt.name.to_s == value.to_s}
      option ||= dictionary.find {|opt| opt.name == default}
      self[property] = option.try(:value)
    end

    define_singleton_method :"#{property}_native" do
      Hash[opts[:options].map.with_index {|opt, i|
        [opt, i + 1]
      }]
    end
  end
end

