#########################
# SplashZoomComponent.rb
# Connor Hilarides
# Created 2014/09/01
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#########################

class SplashZoomComponent < ComponentBase

  serialized_input do |p|
    p.float :speed, default: 2
  end
  
  def initialize(data)
    super data

    @speed = data["speed"] || 2
    @pos = self.owner.transform_component.position

    register_event :update, :on_update
  end

  def on_update(e)
    @pos.z -= e.dt * @speed
  end

  register_component
end
