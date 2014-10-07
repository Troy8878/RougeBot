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


