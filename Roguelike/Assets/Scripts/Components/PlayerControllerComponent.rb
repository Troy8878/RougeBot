##############################
# PlayerControllerComponent.rb
# Jake Robsahm
# Created 2014/09/05
##############################

#########################Class Details##########################
# Component makes the object the player.
#######################Property Details#########################
# health - the health of the player. May be changed if we use
#the same health system for enemies.
# speed - how fast the player moves or attacks. TBD.
# inventory - an inventory object containing all of the player's
#items.
################################################################

PLAYER_INVENTORY = Inventory.new

class PlayerControllerComponent < ComponentBase
  attr_reader :pos

  MIN_MOVE_TIME = 0.2

  KEYS_MOVE_UP = ['W', KeyState::UP]
  KEYS_MOVE_LEFT = ['A', KeyState::LEFT]
  KEYS_MOVE_DOWN = ['S', KeyState::DOWN]
  KEYS_MOVE_RIGHT = ['D', KeyState::RIGHT]

  # Initialize the properties of the PlayerController
  def initialize(data)
    super data

    @transform = self.owner.transform_component
    @pos = @transform.position.dup
    @real_pos = @pos.dup

    # Base player stats. These might be moved to other
    # components later.
    @health = data.fetch("health", 10).to_i
    @speed = data.fetch("speed", 1).to_f

    @move_speed = data.fetch("move_speed", 5).to_f

    self.register_event :key_held, :on_key
    self.register_event :update, :on_update
  end

  def on_key(e)
    case e.plain_char || e.vkey
    when *KEYS_MOVE_UP
      @pos.z += 1 if can_move? 0, 1
    when *KEYS_MOVE_DOWN
      @pos.z -= 1 if can_move? 0, -1
    when *KEYS_MOVE_RIGHT
      @pos.x += 1 if can_move? 1, 0
    when *KEYS_MOVE_LEFT
      @pos.x -= 1 if can_move?(-1, 0)
    end
  end

  def on_update(e)
    diff = @pos - @transform.position.dup
    diff.y = 0

    amt = e.dt * @move_speed
    if diff.length2 > amt*amt
      diff.normalize!.mul amt
    end

    pos = @transform.position
    @transform.position = pos.dup + diff

    xbounce = Math.sin((pos.x % 1) * Math::PI) / 5
    zbounce = Math.sin((pos.z % 1) * Math::PI) / 5

    pos.y = 0.25 + xbounce + zbounce
  end

  def can_move?(xo, yo)
    room = find_entity("MainFloor").test_room_component.room

    # false if the move animation isn't done
    real_pos = @transform.position.dup
    real_pos.y = @pos.y

    return false unless @pos.near? real_pos, 0.2

    x = (@pos.x + 0.5).to_i + xo
    y = (@pos.z + 0.5).to_i + yo

    return false if x < 0 || x >= room[0].count
    return false if y < 0 || y >= room.count

    room[room.count - 1 - y][x] != TestRoomComponent::WALL_VALUE
  end

  register_component "PlayerControllerComponent"
end
