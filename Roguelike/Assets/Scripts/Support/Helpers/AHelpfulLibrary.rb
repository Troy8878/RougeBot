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
