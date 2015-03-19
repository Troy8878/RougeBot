#######################
# ScreenShakeComponent.rb
# Connor Hilarides
# Created 2015/03/19
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class ScreenShakeComponent < ComponentBase

  # Describe what goes in the json file
  serialized_input do |p|
    p.float :strength, default: 0.3
    p.float :length, default: 0.5
    p.string :shake_child, default: "CameraRoot"
  end

  def initialize(data)
    super data

    @strength = data["strength"]
    @length = data["length"]
    @shake_child = data["shake_child"]

    @shaking = false

    register_event :update, :update
  end

  def shake(_ = nil)
    @shaking = true
    @shake_time = 0
    @shake_target = owner.local_find(@shake_child).transform_component
  end

  def update(e)
    return unless @shaking
    @shake_time += e.dt
    if @shake_time > @length
      @shaking = false
      return
    end

    @shake_target.rotation.x = Math.sin(@shake_time)
    @shake_target.rotation.y = Math.cos(@shake_time)
  end

  register_component
end
