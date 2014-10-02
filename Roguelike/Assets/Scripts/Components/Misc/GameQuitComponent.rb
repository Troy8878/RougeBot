#########################
# GameQuitComponent.rb
# Connor Hilarides
# Created 2014/09/01
#########################

class GameQuitComponent < ComponentBase
  def initialize(data)
    super data

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
