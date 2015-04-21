#########################
# GameQuitComponent.rb
# Connor Hilarides
# Created 2014/09/01
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#########################

class GameQuitComponent < ComponentBase

  serialized_input do |p|
    p.bool :disable, default: false
  end
  
  def initialize(data)
    super data

    if data["disable"]
      return
    end

    register_event :key_down, :on_key
  end

  def on_key(e)
    if e.vkey == 27 # [ESC]
      edata = {quit: true}
      Event.raise_event :quit, edata
      Game.quit! if edata[:quit]
    end
  end

  register_component "GameQuitComponent"
end
