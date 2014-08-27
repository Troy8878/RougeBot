#######################
# vector.rb
# Connor Hilarides
# Created 2014/08/24

class Vector
  attr_accessor :x, :y, :z, :w

  def self.zero
    Vector.new 0, 0, 0, 0
  end

  def initialize(x, y, z, w)
    self.x = x
    self.y = y
    self.z = z
    self.w = w
  end
end

