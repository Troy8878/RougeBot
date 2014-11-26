#########################
# PlayerProjectileLogicComponent.rb
# Troy
# Created 2014/10/28
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

class PlayerProjectileLogicComponent < ComponentBase
  include Actor

  attr_accessor :direc, :speed

  def initialize(data)
    super data  # Saving files from corruption since 1996!

    @position = self.owner.position_component.position
    @attack = self.owner.attack_component
    
    @direc = data.fetch("direc", [0, 0])
    @speed = data.fetch("speed", 0)
    @zeroSpeed = false

    register_event :logic_update, :on_update
    register_event :update, :first_update
  end

  def first_update(e)
    # puts "Being Built..."
    actor_init(MapItem::ELLIPSE, "Peru, 0.75") # A colour that in fact does not appear on country-Peru's flag
    actor_minimap_update
    ghost_actor!
    @actor_projectile = true

    if @direc[0] != 0 && @direc[1] != 0
      @actor_diagonal = true
    end

    remove_event :update
  end

  def on_update(e)
    # puts "Preparing to update..."
    return unless actor_init?
    # puts "I am taking an action!"

    # 0 speed just means it doesn't move.
    # This can be achieved by disabling direc instead since speed is used in the collision logic
    # However we still want this projectile to move one tile initially

    if @speed == 0
      @speed = 1
      @zeroSpeed = true
    end

      @logic_cooldown = nil

      # First check the current tile

    if (resolution = can_move?(0, 0)) == false && @move_tile.actor != self.owner # Will need to change to a check for units in the current tile that isn't the player
      # puts "Something is on top of me!!"
      if @blocked_reason == BLOCKED_BY_WALL
        # puts "A wall ran into me"
        delay_sequence(0, true, false)
        # puts "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
        return
      elsif @blocked_reason == BLOCKED_BY_ACTOR && @move_tile.actor.name != "Player"  && @move_tile.actor.name != nil # && @move_tile.actor.actor_projectile == false
        # Move the projectile while attacking.  Otherwise other projectiles will collide with it
        # puts "#{@move_tile.actor.name} ran into me"
        @speed = 1
        @direc[0] = 1
        @direc[1] = 1
        move
        @killed = @move_tile.actor.name
        delay_sequence(0, true, true)
        # puts "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
        return
      elsif @blocked_reason == BLOCKED_BY_COOLDOWN || @blocked_reason == BLOCKED_BY_UNKNOWN || (@blocked_reason == BLOCKED_BY_ACTOR && @move_tile.actor.name == "Player")
        # puts "It was nothing..."
      end
    end

    # Now to check for movement!

    if(@direc != [0, 0])
      for loop in 1..@speed # Need to wait until 1st movement is done to check again!
        if(@speed == 1)
          resolution = can_move? *@direc
        else
          resolution = can_move_more_than_one? *@direc
        end
        # puts "...Moving..."
        if resolution == false && @move_tile.actor != self.owner
        # puts "I ran into something!!"
          if @blocked_reason == BLOCKED_BY_WALL || @blocked_reason == CANNOT_MOVE_DIAGONALLY
            # puts "I ran into a wall"
          # Add a delay:  Don't delete until reaching the last locale that isn't a wall. Do this via  (delay until translation = @position)
            delay_sequence(loop, true, false)
            # puts "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
            return
          elsif @blocked_reason == BLOCKED_BY_ACTOR && @move_tile.actor.name != "Player" && @move_tile.actor != self.owner && @move_tile.actor.name != nil # && @move_tile.actor.actor_projectile == false
            # We have to move into the enemy we're attacking
            # puts "I ran into a #{@move_tile.actor.name}"
            move
            @killed = @move_tile.actor.name
            delay_sequence(loop, true, true)
            # puts "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
            return
          elsif @blocked_reason == BLOCKED_BY_COOLDOWN
            # puts "I moved too soon"
            return
            # puts "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
          # Ignore unknown blocks and the player & move anyways
          elsif @blocked_reason == BLOCKED_BY_UNKNOWN || (@blocked_reason == BLOCKED_BY_ACTOR && @move_tile.actor.name == "Player") # Ignore unknown blocks and the player & move anyways
            # puts "The Player or some unknown force tried to stop me"
            move
            # puts "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
          end
        else
          move
          # puts "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
        end
      end
    end

    # Keep non-moving projectiles "moving" according to the game

    # Resolve 0 Speed projectiles here
    if(@zeroSpeed)
      @direc = [0, 0]
      @zeroSpeed = false
    end

    actor_moved
    resolution = nil
  end

  def move
    @position.x += @direc.at(0)
    @position.y += @direc.at(1)

    actor_minimap_update
  end

  def delay_sequence(rounds, kill, attack)
    seq = self.owner.action_sequence :zombification
    if(@direc[0] != 0 && @direc[1] != 0)
      seq.delay(0.20 * rounds.to_f)
      if(attack)
        seq.once do
          self.owner.attack_component.do_attack @move_tile.actor
        end
      end
      if(kill)
        seq.once do
          self.owner.zombify!
        end
      end
    else
      seq.delay(0.12 * rounds.to_f)
      if(attack)
        seq.once do
          self.owner.attack_component.do_attack @move_tile.actor
        end
      end
      if(kill)
        seq.once do
          self.owner.zombify!
        end
      end
    end
  end

  register_component "PlayerProjectileLogicComponent"
end