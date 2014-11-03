#######################
# Properties.rb
# Connor Hilarides
# Created 2014/11/02
#######################

module MetaProperties
  class Property
    attr_reader :id, :type

    def initialize(id, type, can_set)
      @id = id
      @set = can_set
      @type = type
    end

    def can_set?
      @set
    end

    def get(obj)
      obj.send @id
    end

    def set(obj, val)
      obj.send :"#{@id}=", val
    end
  end

  attr_reader :properties

  def property(id, type, can_set)
    @properties ||= {}

    property = Property.new(id, type, can_set)
    @properties[id] = property
  end
end

class Class
  include MetaProperties
end
