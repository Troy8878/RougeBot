#######################
# AHelpfulLibrary.rb
# Connor Hilarides
# Created 2014/09/09
#######################

module Kernel
  def method(sym)
    Proc.new {|*args| self.send sym, *args }
  end
end

module Math
  def Math.signum(i)
    -1 if i < 0
    1 if i > 0
    0
  end

  def Math.round(i)
    return (i + 0.5).to_i if i >= 0
    return (i - 0.5).to_i
  end

  def Math.abs(i)
    i < 0 ? -i : i
  end
end

module Enumerable
  def size
    length
  end

  def count
    length
  end
end

class Array
  def sum
    total = first
    self[1..-1].each do |item|
      total += item
    end
    total
  end

  def average
    sum / count
  end
end

module Enumerable
  def init
    lazy.take(count - 1).to_a
  end

  def tail
    lazy.drop(1).to_a
  end

  def enum_at(i)
    self.each do |item|
      return item if i == 0
      i -= 1
    end
    nil
  end

  def first
    enum_at 0
  end

  def second
    enum_at 1
  end

  def third
    enum_at 2
  end

  def fourth
    enum_at 3
  end

  def fifth
    enum_at 4
  end
end

class String
  def char_code
    bytes[0]
  end
end

class Symbol
  def to_proc
    -> (a1, *args) do
      a1.send self, *args
    end
  end
end

