#######################
# SpinnyCameraComponent.rb
# Connor Hilarides
# Created 2014/09/09
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#######################

class SpinnyCameraComponent < ComponentBase
  attr_accessor :speed
  dependency "CameraComponent"

  serialized_input do |p|
    p.dependency "CameraComponent"
    p.float :speed, default: 1
    p.float :distance, default: 1
  end

  def initialize(data)
    super data

    @angle = 0
    @speed = data.fetch("speed", 1).to_f
    @distance = data.fetch("distance", 1).to_f
    @camera = self.owner.camera_component

    self.register_event :update, :on_update
  end

  def on_update(e)
    @angle -= e.dt * @speed

    @camera.position.x = Math.sin(@angle) * @distance
    @camera.position.z = Math.cos(@angle) * @distance
  end

  register_component "SpinnyCameraComponent"
end
