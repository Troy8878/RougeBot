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

    register_event :logic_update, :on_update
    register_event :update, :first_update
  end

  def first_update(e)
    actor_init(MapItem::ELLIPSE, "Peru, 0.7") # A colour that in fact does not appear on country-Peru's flag
    actor_minimap_update
    ghost_actor!

    remove_event :update
  end

  def on_update(e)
    return unless actor_init?

    for loop in 1..@speed
      @logic_cooldown = nil
      resolution = can_move? *@direc
      puts resolution.to_s
      calc_x = @position.x += @direc.at(0)
      calc_y = @position.y += @direc.at(1)
      puts "#{calc_x}, #{calc_y}"

      if resolution == false
        if @blocked_reason == BLOCKED_BY_WALL || @blocked_reason == BLOCKED_BY_UNKNOWN
          self.owner.zombify!
          puts @blocked_reason.to_s
          return
        elsif @blocked_reason == BLOCKED_BY_ACTOR && @move_tile.actor.name != "Player"
          self.owner.attack_component.do_attack @move_tile.actor
          self.owner.zombify!
          puts @blocked_reason.to_s
          return
        end
      else
        move
      end
    end

    actor_moved
  end

  def move

    @position.x += @direc.at(0)
    @position.y += @direc.at(1)
    puts "Moved"

    actor_minimap_update
  end

  register_component "PlayerProjectileLogicComponent"
end
