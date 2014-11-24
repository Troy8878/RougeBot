#######################
# GoToSplash.rb
# Connor Hilarides
# Created 2014/11/14
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class GoToSplash < ComponentBase
  def initialize(data)
    super data

    register_event :update, :first_update
  end

  def first_update(e)
    Game.switch_level "Splash"
  end

  register_component
end
