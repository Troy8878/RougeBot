#######################
# CameraFollowComponent.rb
# Connor Hilarides
# Created 2014/09/09
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class CameraFollowComponent < ComponentBase
  attr_accessor :offset, :lag

  def initialize(data)
    super data

    @transform = self.owner.transform_component

    @offset = Vector.new(*data.fetch("offset", [1, 0, 1]))

    @speed = data.fetch("speed", 0.0).to_f
    @superspeed = data.fetch("superspeed", 3).to_f
    @follow = Vector.new
    @follow_id = data.fetch("follows", 0)
    @follow_id = @follow_id.to_i if @follow_id.is_a? Float

    register_event :update, :on_update
  end

  def on_update(e)
    if @follow_tr.nil?
      @follow_tr = find_entity(@follow_id).transform_component
      @follow_real = @follow_tr.position.dup
    end

    follow = @follow_tr.position.dup

    if @speed > 0.0
      diff = follow - @follow_real
      modifier = e.dt * @speed
      dlen2 = diff.length2

      if dlen2 > @superspeed * @superspeed
        modifier *= (1 + (diff.length - @superspeed))
      end
      
      if dlen2 > (modifier * modifier)
        diff.normalize!
        diff.mul modifier
      end

      follow = @follow_real + diff

      @follow_real = follow
    end

    follow.y = 0.25
    @transform.position = follow + @offset
  end

  register_component "CameraFollowComponent"
end
