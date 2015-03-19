#######################
# ScreenShakeComponent.rb
# Connor Hilarides
# Created 2015/03/19
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class ScreenShakeComponent < ComponentBase

  # Describe what goes in the json file
  serialized_input do |p|
    p.float :frequency, default: 12
    p.float :strength, default: 0.5
    p.float :length, default: 1
    p.string :shake_child, default: "CameraRoot"
  end

  def initialize(data)
    super data

    @frequency = data["frequency"].to_f
    @strength = data["strength"].to_f
    @length = data["length"].to_f
    @shake_child = data["shake_child"]
  end

  def shake(modifier = 1.0)
    shake_target = owner.local_find(@shake_child).transform_component

    if @shake_child == "."
      modifier = 2.2 - Math::E
    end

    strength = @strength * Math.log(modifier + Math::E - 1)
    length = @length * Math.log(modifier + Math::E - 1)

    pos = shake_target.position
    seq = owner.action_sequence :screen_shake
    interval = 1.0 / @frequency

    orig_pos = pos.dup
    prev_pos = pos.dup
    (0 .. @frequency * length).each do |i|
      new_pos = Vector.new(
        pos.x + Random.float_range(-strength, strength),
        pos.y + Random.float_range(-strength, strength),
        pos.z, 1
      )

      seq.interpolate pos, from: prev_pos, to: new_pos, over: interval
      prev_pos = new_pos
    end

    seq.interpolate pos, from: prev_pos, to: orig_pos, over: interval
  end

  register_component
end
