#######################
# SpinnyThingComponent.rb
# Connor Hilarides
# Created 2014/08/25
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#######################

class SpinnyThingComponent < ComponentBase
  dependency "TransformComponent"
  attr_accessor :speed
  attr_reader :axis

  serialized_input do |p|
    p.dependency "TransformComponent"
    p.float :speed, default: 1
    p.enum  :axis, options: ["x", "y", "z"], default: "z"
  end

  def initialize(data)
    super data

    @speed = data.fetch("speed", 1).to_f
    @transform = self.owner.transform_component
    @rotation = @transform.rotation
    self.axis = data.fetch("axis", "z")

    self.register_event :update, :on_update
  end

  def on_update(e)
    @rotation.send(
      @set_axis,
      @rotation.send(@get_axis) + e.dt * speed
    )
  end

  def axis=(a)
    @axis = a
    @get_axis = @axis.to_sym
    @set_axis = "#{@axis}=".to_sym
  end

  register_component "SpinnyThingComponent"
end
