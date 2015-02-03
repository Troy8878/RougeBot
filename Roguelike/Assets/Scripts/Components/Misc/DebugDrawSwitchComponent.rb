#######################
# DebugDrawSwitchComponent.rb
# Connor Hilarides
# Created 2014/09/01
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class DebugDrawSwitchComponent < ComponentBase

  serialized_input do |p|
  end
  
  def initialize(data)
    super data

    register_event :key_down, :key_down
  end

  def key_down(e)
    if e.vkey == 120
      Game.toggle_debug_draw
    end
  end

  register_component "DebugDrawSwitchComponent"
end
