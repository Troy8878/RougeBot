#########################
# SplashLevelSwitcher.rb
# Connor Hilarides
# Created 2014/09/01
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

class SplashLevelSwitcher < ComponentBase

  serialized_input do |p|
  end
  
  def initialize(data)
    super data

    @time = 0.0

    register_event :update, :on_update
  end

  def on_update(e)
    @time += e.dt

    if @time > 4
      Game.switch_level "MainMenu"
    end
  end

  register_component "SplashLevelSwitcher"
end
