#########################
# PlayerProjectileLogicComponent.rb
# Troy
# Created 2014/10/28
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

    # 0 speed just means it doesn't move.
    #This can be achieved by disabling direc instead since speed is used in the collision logic

    if @speed == 0
      @speed = 1
      @direc = [0, 0]
    end

    register_event :logic_update, :on_update
    register_event :update, :first_update
  end

  def first_update(e)
    actor_init(MapItem::ELLIPSE, "Peru, 0.75") # A colour that in fact does not appear on country-Peru's flag
    actor_minimap_update
    ghost_actor!

    remove_event :update
  end

  def on_update(e)
    return unless actor_init?

      @logic_cooldown = nil

      # First check the current tile

    if true == false # Will need to change to a check for units in the current tile that isn't the player or projectiles
      self.owner.zombify!
      return
    elsif @blocked_reason == BLOCKED_BY_ACTOR && @move_tile.actor.name != "Player"
      # We have to move into the enemy we're attacking
      move
      self.owner.attack_component.do_attack @move_tile.actor
      #seq = self.owner.action_sequence :zombification
      #seq.delay(0.15)
      #seq.once do
      self.owner.zombify!
      #end
      return
    elsif @blocked_reason == BLOCKED_BY_COOLDOWN
      puts "Code Error"
      return
    elsif @blocked_reason == BLOCKED_BY_UNKNOWN || (@blocked_reason == BLOCKED_BY_ACTOR && @move_tile.actor.name == "Player") # Ignore unknown blocks and the player & move anyways
      move
    end

    # Now to check for movement!

    for loop in 1..@speed # Need to wait until 1st movement is done to check again!
      if(@speed == 1)
        resolution = can_move? *@direc
      else
        resolution = can_move_more_than_one? *@direc
      end

      if resolution == false
        if @blocked_reason == BLOCKED_BY_WALL # Add a delay:  Don't delete until reaching the last locale that isn't a wall.
                                              # Do this via  (delay until translation = @position)
          self.owner.zombify!
          return
        elsif @blocked_reason == BLOCKED_BY_ACTOR && @move_tile.actor.name != "Player"
          # We have to move into the enemy we're attacking
          move
          self.owner.attack_component.do_attack @move_tile.actor
          #seq = self.owner.action_sequence :zombification
          #seq.delay(0.15)
          #seq.once do
          self.owner.zombify!
          #end
          return
        elsif @blocked_reason == BLOCKED_BY_COOLDOWN
          puts "Code Error"
          return
        elsif @blocked_reason == BLOCKED_BY_UNKNOWN || (@blocked_reason == BLOCKED_BY_ACTOR && @move_tile.actor.name == "Player") # Ignore unknown blocks and the player & move anyways
          move
        end
      else
        move

      end
    end

    actor_moved
    resolution = nil

  end

  def move
    @position.x += @direc.at(0)
    @position.y += @direc.at(1)

    actor_minimap_update
  end

  register_component "PlayerProjectileLogicComponent"
end
