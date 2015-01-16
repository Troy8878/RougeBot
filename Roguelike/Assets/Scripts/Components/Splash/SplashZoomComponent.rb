#########################
# SplashZoomComponent.rb
# Connor Hilarides
# Created 2014/09/01
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

class SplashZoomComponent < ComponentBase
  def initialize(data)
    super data

    @transform = self.owner.transform_component

    register_event :update, :on_update
  end

  def on_update(e)
    @transform.position.z -= e.dt * 2
  end

  register_component
end
