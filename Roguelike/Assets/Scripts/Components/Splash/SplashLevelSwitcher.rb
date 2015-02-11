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

    seq = owner.action_sequence :next_level
    seq.delay 4.0
    seq.once do
      Game.switch_level "MainMenu"
    end
  end

  register_component "SplashLevelSwitcher"
end
