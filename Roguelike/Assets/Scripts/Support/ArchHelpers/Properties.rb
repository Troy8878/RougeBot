#######################
# Properties.rb
# Connor Hilarides
# Created 2014/11/02
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#######################

module MetaProperties
  class MetaProperty
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

  def property(id, type, can_set = false)
    @properties ||= []

    property = MetaProperty.new(id, type, can_set)
    @properties << property
  end
end

class Class
  include MetaProperties
end

##
# Give vector some properties
class Vector
  property :x, :float, true
  property :y, :float, true
  property :z, :float, true
  property :w, :float, true
end
