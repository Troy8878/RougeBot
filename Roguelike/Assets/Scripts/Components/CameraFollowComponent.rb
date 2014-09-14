#######################
# CameraFollowComponent.rb
# Connor Hilarides
# Created 2014/09/09
#######################

class CameraFollowComponent < ComponentBase
  attr_accessor :offset, :lag

  def initialize(data)
    super data

    @transform = self.owner.transform_component

    @offset = Vector.new(*data.fetch("offset", [1, 0, 1]))

    @lag = data.fetch("lag", 0.0).to_f
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

    if @lag > 0.0
      follow += (@follow_real - follow) / (1 / @lag + 1)
      @follow_real = follow
    end

    @transform.position = follow + @offset
  end

  register_component "CameraFollowComponent"
end
