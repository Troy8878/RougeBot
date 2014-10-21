#########################
# PositionComponent.rb
# Connor Hilarides
# Created 2014/09/16
#########################

class PositionComponent < ComponentBase
  attr_accessor :position

  def initialize(data)
    super data

    @mode = data.fetch("mode", :jump).to_sym
    @move_speed = data.fetch("move_speed", 5).to_f
    @position = Vector.new *data.fetch("position", [])

    @tpos = self.owner.transform_component.position

    register_event :update, :update
  end

  def update(e)
    return unless respond_to? @mode

    self.send(@mode, e.dt)
  end

  def jump(dt)
    @tpos.x = @position.x
    @tpos.z = @position.y
  end

  def linear(dt)
    real_pos = @tpos.dup
    real_pos.y = real_pos.z

    diff = @position - real_pos
    diff.z = 0

    amt = dt * @move_speed
    if diff.length2 > amt*amt
      diff.normalize!.mul amt
    end

    @tpos.x = real_pos.x + diff.x
    @tpos.z = real_pos.y + diff.y
  end

  def exponential(dt)
    real_pos = @tpos.dup
    real_pos.y = real_pos.z

    diff = @position - real_pos
    diff.z = 0

    diff.mul(dt * 50 / @move_speed)

    @tpos.x = real_pos.x + diff.x
    @tpos.z = real_pos.y + diff.y
  end

  register_component "PositionComponent"
end