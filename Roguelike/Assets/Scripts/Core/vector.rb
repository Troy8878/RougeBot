#######################
# vector.rb
# Connor Hilarides
# Created 2014/08/24
#######################

class Vector
  def self.zero; Vector.new(0, 0, 0, 0); end
  def self.origin; Vector.new(0, 0, 0, 1); end

  def self.left; Vector.new(-1, 0, 0, 1); end
  def self.right; Vector.new(1, 0, 0, 1); end

  def self.down; Vector.new(0, -1, 0, 1); end
  def self.up; Vector.new(0, 1, 0, 1); end

  def self.backward; Vector.new(0, 0, -1, 1); end
  def self.forward; Vector.new(0, 0, 1, 1); end

  def +(v)
    c = Vector.new self
    c.add(v)
  end

  def -(v)
    c = Vector.new self
    c.sub(v)
  end

  def *(v)
    c = Vector.new self
    c.mul(v)
  end

  def /(v)
    c = Vector.new self
    c.div(v)
  end

  def inspect
    "<#{x}, #{y}, #{z}, #{w}>"
  end

  def to_s
    self.inspect
  end
end

