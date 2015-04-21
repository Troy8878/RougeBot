#########################
# EnemyLogicComponent.rb
# Claire Robsahm, Enrique Rodriguez
# Created 2014/10/20
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#########################

class EnemyLogicComponent < ComponentBase
  include Actor
  attr_accessor :stationary

  dependency "PositionComponent", "AttackComponent"

  serialized_input do |p|
    p.dependency "PositionComponent"
    p.dependency "AttackComponent"
    p.bool    :stationary, default: false
    p.integer :move_skip
    p.bool    :new_system, default: false
  end

  def initialize(data)
    super data

    @position = self.owner.position_component.position
    @attack = self.owner.attack_component

    @stationary = !!data["stationary"]
    @move_skip = data.fetch("move_skip", 1).to_i
    @new_system = data.fetch("new_system", false)
    @skip_number = 0

    register_event :logic_update, :on_update
    register_event :update, :first_update
  end

  def first_update(e)
    maptype = [MapItem::ELLIPSE, "Red"]

    if @stationary
      maptype = [MapItem::RECTANGLE, "CornflowerBlue, 0.7"]
    end

    actor_init(maptype[0], maptype[1])
    actor_minimap_update

    actor_moved

    remove_event :update
  end

  def on_update(player)
    return if @stationary
    return if @new_system

    position_player = player.position_component.position
    distance = @position.distance position_player

    if distance < 1.1
      @attack.do_attack player if @attack
    elsif distance < 3
      @skip_number += 1
      if @skip_number >= @move_skip
        @skip_number = 0
      end

      if @skip_number == 0
        move_towards player
      end
    else
      move_randomly
    end
  end

  def move_randomly()
    direction = Random.die_roll 4
    
    if(direction == 1)
      if can_move?(1, 0)
        move 1, 0
      end
    elsif(direction == 2)
      if can_move?(-1, 0)
        move -1, 0
      end
    elsif(direction == 3)
      if can_move?(0, 1)
        move 0, 1
      end
    elsif(direction == 4)
      if can_move?(0, -1)
        move 0, -1
      end
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

  ###############################################################
  # This code won't do anything.  Projectiles are ghost actors. #
  ###############################################################

  PROJECTILE_NAMES = [
    "Bomb",
    "Mine",
    "Arrow"
  ]

  def resolve_player_projectiles?(x, y)
    return unless @move_tile && @move_tile.actor
    return unless PROJECTILE_NAMES.include? @move_tile.actor.name

    @move_tile.actor.owner.attack_component.do_attack self
    @move_tile.actor.owner.PlayerProjectileLogicComponent.decay_sqeuence(1)

  #######################
  # End of useless code #
  #######################

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
