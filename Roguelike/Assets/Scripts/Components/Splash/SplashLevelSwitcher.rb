#########################
# SplashLevelSwitcher.rb
# Connor Hilarides
# Created 2014/09/01
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#########################

class SplashLevelSwitcher < ComponentBase

  serialized_input do |p|
    p.float :splash_time, default: 4.0
    p.string :level, default: "MainMenu"
  end
  
  def initialize(data)
    super data

    @cancelled = false

    seq = owner.action_sequence :next_level
    seq.delay data.fetch("splash_time", 4.0).to_f
    seq.once do
      next if @cancelled
      Game.switch_level data["level"]
    end

    register_event :key_down, :key_down
    register_event :mouse_down, :key_down
  end

  def key_down(e)
    @cancelled = true
    Game.switch_level "MainMenu"
  end

  register_component "SplashLevelSwitcher"
end
