###########################
# ButtonAdjustment.rb
# Connor Hilarides
# Created 2014/10/24
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
###########################

class ButtonAdjustment < ComponentBase

  serialized_input do |p|
  end
  
  def initialize(data)
    super data

    @data = data

    register_event :update, :first_update
  end

  def first_update(e)
    remove_event :update

    text = owner.local_find "ButtonText"
    background = owner.local_find "ButtonBackground"
  end

  register_component "ButtonAdjustment"
end
