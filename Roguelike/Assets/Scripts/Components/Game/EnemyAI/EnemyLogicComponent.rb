#########################
# EnemyLogicComponent.rb
# Jake Robsahm
# Created 2014/10/20
#########################

class EnemyLogicComponent < ComponentBase
  include Actor

  def initialize(data)
    super data

    @position = self.owner.position_component.position
    @attack = self.owner.attack_component

    register_event :logic_update, :on_update
    register_event :update, :first_update
  end

  def first_update(e)
    actor_init(MapItem::ELLIPSE, "Red")
    actor_minimap_update

    remove_event :update
  end

  def on_update(player)
    position_player = player.position_component.position
    distance = @position.distance position_player

    if distance < 1.1
      @attack.do_attack player
    elsif distance < 20
      move_towards player
    end
  end

  def move_towards(target)
    difference = @position - target.position_component.position
    difference.normalize!

    dx = difference.x
    dy = difference.y
    ax = Math.abs dx
    ay = Math.abs dy

    if ax > ay
      if dx > 0
        move -1, 0
      else
        move 1, 0
      end
    else
      if dy > 0
        move 0, -1
      else
        move 0, 1
      end
    end
  end

  def move(dx, dy)
    @position.x += dx
    @position.y += dy

    actor_moved
  end

  register_component "EnemyLogicComponent"
end
