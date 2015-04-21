#######################
# ButtonSoundsComponent.rb
# Connor Hilarides
# Created 2014/12/05
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#######################

class ButtonSoundsComponent < ComponentBase

  serialized_input do |p|
  end
  
  def initialize(data)
    super data

    register_event :button_clicked, :clicked
    register_event :mouse_enter, :hover
  end
  
  def clicked(e)
    SFX::HIGH_CLICK.play
  end

  def hover(e)
    SFX::LOW_TICK.play
  end

  register_component
end
