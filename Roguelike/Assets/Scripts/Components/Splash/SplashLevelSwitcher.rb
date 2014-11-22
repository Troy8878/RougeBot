#########################
# SplashLevelSwitcher.rb
# Connor Hilarides
# Created 2014/09/01
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

class SplashLevelSwitcher < ComponentBase
  def initialize(data)
    super data

    @time = 0.0

    # Prime the GC
    ary = []
    1000.times do
      ary << Object.new
    end

    register_event :update, :on_update
  end

  def on_update(e)
    @time += e.dt

    if @time > 0.1
      Game.switch_level "MainMenu"
    end
  end

  register_component "SplashLevelSwitcher"
end
