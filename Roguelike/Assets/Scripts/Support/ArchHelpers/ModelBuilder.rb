#######################
# ModelBuilder.rb
# Connor Hilarides
# Created 2014/09/09
#######################

module ModelBuilder
  class Vertex
    attr_accessor :position, :color, :texture

    def initialize(position = nil, color = nil, texture = nil)
      @position = position || Vector.new
      @color    = color    || Vector.new
      @texture  = texture  || Vector.new
    end
  end
end
