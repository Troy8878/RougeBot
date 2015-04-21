#######################
# Item.rb
# Connor Hilarides
# Created 2014/09/05
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#######################

class ItemAttribute
  attr_reader :category, :value

  def initialize(category, value)
    @category = category
    @value = value
  end
end
