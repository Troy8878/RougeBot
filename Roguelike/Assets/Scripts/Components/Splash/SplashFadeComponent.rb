#########################
# SplashZoomComponent.rb
# Connor Hilarides
# Created 2014/09/01
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

class SplashZoomComponent < ComponentBase
  dependency "SpriteComponent"

  serialized_input do |p|
    p.float :fade_time, default: 1.0
    p.float :wait_time, default: 3.0
  end
  
  def initialize(data)
    super data

    fade_time = data.fetch("fade_time", 1.0).to_f
    wait_time = data.fetch("wait_time", 3.0).to_f
    @sprite = owner.sprite_component

    seq = owner.action_sequence :fade
    seq.interpolate @sprite.tint, from: Vector.ZERO, to: Vector.ONE, over: fade_time
    seq.delay wait_time
    seq.interpolate @sprite.tint, from: Vector.ONE, to: Vector.ZERO, over: fade_time
  end

  register_component
end
