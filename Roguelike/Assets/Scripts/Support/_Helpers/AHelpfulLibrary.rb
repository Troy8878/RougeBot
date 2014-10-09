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

  def first
    return nil if count < 1
    lazy.each{break}.first
  end

  def second
    return nil if count < 2
    lazy.drop(1).each{break}.first
  end

  def third
    return nil if count < 3
    lazy.drop(2).each{break}.first
  end

  def fourth
    return nil if count < 4
    lazy.drop(3).each{break}.first
  end

  def fifth
    return nil if count < 5
    lazy.drop(4).each{break}.first
  end
end

class String
  def char_code
    bytes[0]
  end
end


