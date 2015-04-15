#########################
# SplashFadeComponent.rb
# Connor Hilarides
# Created 2015/04/15
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

class SplashFadeComponent < ComponentBase
  dependency "SpriteComponent"

  serialized_input do |p|
    p.float :pre_wait, default: 2.0
    p.float :fade_time, default: 1.0
    p.float :wait_time, default: 3.0
  end
  
  def initialize(data)
    super data

    pre_wait = data.fetch("pre_wait", 2.0).to_f
    fade_time = data.fetch("fade_time", 1.0).to_f
    wait_time = data.fetch("wait_time", 3.0).to_f
    @sprite = owner.sprite_component

    start = Vector.new(0, 0, 0, 0.5)
    finish = Vector::ONE
    @sprite.tint = start

    seq = owner.action_sequence :fade
    seq.delay pre_wait
    seq.interpolate @sprite.tint, from: start, to: finish, over: fade_time
    seq.delay wait_time
    seq.interpolate @sprite.tint, from: finish, to: start, over: fade_time
  end

  register_component
end
