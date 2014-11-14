#########################
# SplashLevelSwitcher.rb
# Connor Hilarides
# Created 2014/09/01
#########################

class SplashLevelSwitcher < ComponentBase
  def initialize(data)
    super data

    @time = 0.0

    register_event :update, :on_update
  end

  def on_update(e)
    @time += e.dt

    if @time > 0.1
      Game.switch_level "Game"
    end
  end

  register_component "SplashLevelSwitcher"
end
