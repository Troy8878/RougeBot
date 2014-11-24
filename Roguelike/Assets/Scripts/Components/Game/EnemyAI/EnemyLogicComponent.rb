#########################
# EnemyLogicComponent.rb
# Jake Robsahm, Enrique Rodriguez
# Created 2014/10/20
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
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

    actor_moved

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
        if can_move?(-1, 0)
          move -1, 0
        elsif ay
          if dy > 0
            move 0, -1
          else
            move 0, 1
          end
        end
      else
        if can_move?(1, 0)
          move 1, 0
        elsif ay
          if dy > 0
            move 0, -1
          else
            move 0, 1
          end
        end
      end
    else
      if dy > 0
        if can_move?(0, -1)
          move 0, -1
        elsif ax
          if dx > 0
            move -1, 0
          else
            move 1, 0
          end
        end
      else
        if can_move?(0, 1)
          move 0, 1
        elsif ax
          if dx > 0
            move -1, 0
          else
            move 1, 0
          end
        end
      end
    end
  end

  PROJECTILE_NAMES = [
    "Bomb",
    "Mine",
    "Arrow"
  ]

  # This won't do anything for the moment.  Projectiles are ghost actors.
  def resolve_player_projectiles?(x, y)
    return unless @move_tile && @move_tile.actor
    return unless PROJECTILE_NAMES.include? @move_tile.actor.name

    @move_tile.actor.owner.attack_component.do_attack self
    @move_tile.actor.owner.PlayerProjectileLogicComponent.decay_sqeuence(1)
  end

  def move(dx, dy)
    return unless can_move? dx, dy

    resolve_player_projectiles?(dx, dy)

    @position.x += dx
    @position.y += dy

    #resolve_player_projectiles_past?(dx, dy)

    actor_moved
  end

  register_component "EnemyLogicComponent"
end
