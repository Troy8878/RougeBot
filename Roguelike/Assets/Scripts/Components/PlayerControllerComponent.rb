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
    self.register_event :update, :first_update
    self.register_event :mouse_down, :mouse_down
  end

  def on_key(e)
    case e.plain_char || e.vkey
    when *KEYS_MOVE_UP
      move 0, +1 if can_move? 0, 1
    when *KEYS_MOVE_DOWN
      move 0, -1 if can_move? 0, -1
    when *KEYS_MOVE_RIGHT
      move +1, 0 if can_move? 1, 0
    when *KEYS_MOVE_LEFT
      move -1, 0 if can_move? -1, 0
    end
  end

  def mouse_down(e)
    @cursor ||= find_entity("TileCursor")

    curpos = @cursor.transform_component.position
    dx = Math.round(curpos.x - @transform.position.x)
    dz = Math.round(curpos.z - @transform.position.z)

    return if Math.abs(dx) > 1.5 || Math.abs(dz) > 1.5

    move dx, dz if can_move? dx, dz
  end

  def move(x, z)
    @pos.x += x
    @pos.z += z
    @minimap ||= find_entity("Minimap")
    @minimap.raise_event :map_update, nil
  end

  def first_update(e)
    move 0, 0

    register_event :update, :on_update
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

    xbounce = Math.sin((pos.x % 1) * Math::PI) / 10
    zbounce = Math.sin((pos.z % 1) * Math::PI) / 10

    pos.y = 0.25 + xbounce + zbounce

    update_cursor_color
  end

  def update_cursor_color
    @cursor ||= find_entity("TileCursor")

    curpos = @cursor.transform_component.position
    dx = Math.round(curpos.x - @pos.x)
    dz = Math.round(curpos.z - @pos.z)

    if Math.abs(dx) > 1.5 || Math.abs(dz) > 1.5 || !can_move?(dx, dz)
      @cursor.children.first.sprite_component.texture_index = 1
    else
      @cursor.children.first.sprite_component.texture_index = 0
    end
  end

  def can_move?(xo, yo)
    if xo != 0 and yo != 0
      return false unless can_move?(xo, 0) && can_move?(0, yo)
    end

    @room ||= find_entity("MainFloor").test_room_component.room
    room = @room

    # false if the move animation isn't done
    real_pos = @transform.position.dup
    real_pos.y = @pos.y

    return false unless @pos.near? real_pos, 0.2

    x = (@pos.x + 0.5).to_i + xo
    y = (@pos.z + 0.5).to_i + yo

    return false if x < 0 || x >= room[0].count
    return false if y < 0 || y >= room.count

    room[room.count - 1 - y][x] == TestRoomComponent::EMPTY_VALUE
  end

  register_component "PlayerControllerComponent"
end
