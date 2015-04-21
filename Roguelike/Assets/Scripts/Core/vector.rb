#######################
# vector.rb
# Connor Hilarides
# Created 2014/08/24
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#######################

class Vector
  alias r x
  alias g y
  alias b z
  alias a w
  alias r= x=
  alias g= y=
  alias b= z=
  alias a= w=

  ZERO = Vector.new     0, 0, 0, 0
  ONE = Vector.new      1, 1, 1, 1
  ORIGIN = Vector.new   0, 0, 0, 1

  LEFT = Vector.new    -1, 0, 0, 0
  RIGHT = Vector.new    1, 0, 0, 0

  DOWN = Vector.new     0,-1, 0, 0
  UP = Vector.new       0, 1, 0, 0

  BACKWARD = Vector.new 0, 0,-1, 0
  FORWARD = Vector.new  0, 0, 1, 0

  def self.zero; ZERO; end
  def self.one; ONE; end
  def self.origin; ORIGIN; end

  def self.left; LEFT; end
  def self.right; RIGHT; end

  def self.down; DOWN; end
  def self.up; UP; end

  def self.backward; BACKWARD; end
  def self.forward; FORWARD; end

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

  def to_hex_color
    "##{x.to_hex_color}#{y.to_hex_color}#{z.to_hex_color}#{w.to_hex_color}"
  end

  def self.from_color(p, default = nil)
    if p.is_a? Vector
      p
    elsif p.is_a? String
      Vector.string_to_color p
    elsif p.is_a? Array
      Vector.new *p
    else
      default || Vector.one
    end
  end

  def to_a
    [x, y, z, w]
  end
end

class Float
  def to_hex_color
    f = self
    f = 0 if f < 0
    f = 1 if f > 1
    f = f * 255
    f = f.to_i
    f = f.to_s 16
    f = "0#{f}" if f.length < 2
    f.upcase
  end
end

