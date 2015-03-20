#########################
# SplashLevelSwitcher.rb
# Connor Hilarides
# Created 2014/09/01
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

class SplashLevelSwitcher < ComponentBase

  serialized_input do |p|
    p.float :splash_time, default: 4.0
  end
  
  def initialize(data)
    super data

    seq = owner.action_sequence :next_level
    seq.delay data.fetch("splash_time", 4.0).to_f
    seq.once do
      Game.switch_level "MainMenu"
    end
  end

  register_component "SplashLevelSwitcher"
end
