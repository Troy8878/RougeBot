#######################
# vector.rb
# Connor Hilarides
# Created 2014/08/24
#######################

class Vector
  attr_accessor :x, :y, :z, :w

  def self.zero; Vector.new 0, 0, 0, 0; end
  def self.origin; Vector.new 0, 0, 0, 1; end

  def self.left; Vector.new -1, 0, 0, 1; end
  def self.right; Vector.new 1, 0, 0, 1; end

  def self.down; Vector.new 0, -1, 0, 1; end
  def self.up; Vector.new 0, 1, 0, 1; end

  def self.backward; Vector.new 0, 0, -1, 1; end
  def self.forward; Vector.new 0, 0, 1, 1; end

  def initialize(x = 0, y = 0, z = 0, w = 0)
    @x = x
    @y = y
    @z = z
    @w = w
  end
end

