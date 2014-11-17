#########################
# PositionComponent.rb
# Connor Hilarides
# Created 2014/09/16
#########################

class PositionComponent < ComponentBase
  attr_accessor :position, :move_speed

  property :position, :vector
  property :move_speed, :float, true

  def initialize(data)
    super data

    @mode = data.fetch("mode", :jump).to_sym
    @move_speed = data.fetch("move_speed", 5).to_f
    @position = Vector.new *data.fetch("position", [])

    @tpos = self.owner.transform_component.position

    self.jump(0)

    register_event :update, :update
  end

  def update(e)
    return unless respond_to? @mode

    self.send(@mode, e.dt)
  end

  def jump(dt)
    @tpos.x = @position.x
    @tpos.y = @position.y
  end

  def linear(dt)
    return if @position.near? @tpos, 0.0001

    diff = @position - @tpos
    diff.z = 0

    amt = dt * @move_speed
    if diff.length2 > amt*amt
      diff.normalize!.mul amt
    end

    @tpos.x += diff.x
    @tpos.y += diff.y
  end

  def exponential(dt)
    diff = @position - @tpos
    diff.z = 0

    diff.mul(dt * 50 / @move_speed)

    @tpos.x += diff.x
    @tpos.y += diff.y
  end

  register_component "PositionComponent"
end
