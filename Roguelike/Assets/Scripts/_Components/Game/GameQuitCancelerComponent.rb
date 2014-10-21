#########################
# GameQuitCancelerComponent.rb
# Connor Hilarides
# Created 2014/09/01
#########################

class GameQuitCancelerComponent < ComponentBase
  def initialize(data)
    super data

    register_event :quit, :on_quit
  end

  def on_quit(e)
    e[:quit] = false
    Game.switch_level "Splash"
  end

  register_component "GameQuitCancelerComponent"
end
