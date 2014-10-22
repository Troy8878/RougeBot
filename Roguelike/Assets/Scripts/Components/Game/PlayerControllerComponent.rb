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
# inventory - an inventory object containing all of the player's
#items.
################################################################

PLAYER_INVENTORY = Inventory.new

class PlayerControllerComponent < ComponentBase
  include Actor

  attr_reader :pos

  MIN_MOVE_TIME = 0.2

  BLOCKED_BY_UNKNOWN = 0
  BLOCKED_BY_ACTOR = 1
  BLOCKED_BY_WALL = 2
  BLOCKED_BY_COOLDOWN = 3

  # Initialize the properties of the PlayerController
  def initialize(data)
    super data

    @transform = self.owner.transform_component
    @pos = self.owner.position_component.position

    @logic_initialized = false
    @logic_cooldown = 0

    self.register_event :player_move, :on_move
    self.register_event :update, :first_update

    # Double-click should do it in either case
    # Double-click is the only way in touch mode
    self.register_event :double_click, :mouse_down
    unless Config[:touch_mode]
      self.register_event :mouse_down, :mouse_down
    end

    self.register_event :mouse_move, :mouse_move
  end

  def move(x, y)
    unless can_move? x, y
      return if @blocked_reason != BLOCKED_BY_ACTOR
    end

    if @blocked_reason == BLOCKED_BY_ACTOR
      self.owner.attack_component.do_attack @move_tile.actor
      @logic_cooldown += 0.5
    else
      @pos.x += x
      @pos.y += y

      actor_moved
    end

    if @logic_initialized
      Event.raise_event :logic_update, self.owner
    end
    
    actor_minimap_refresh
  end

  def on_move(e)
    set_kb_mode
    move *e
  end

  def mouse_down(e)
    @cursor ||= find_entity("TileCursor")
    @curpos ||= @cursor.transform_component.position

    dx = Math.round(@curpos.x - @pos.x)
    dy = Math.round(@curpos.z - @pos.y)

    move dx, dy
  end

  def first_update(e)
    shape = MapItem::ELLIPSE
    actor_init(shape, "Yellow")

    move 0, 0

    @camera = find_entity("CameraRoot")
    @camera.parent = self.owner
    @camz = @camera.transform_component.position.z

    @logic_initialized = true

    set_kb_mode

    register_event :update, :on_update
  end

  def on_update(e)
    pos = @transform.position

    xbounce = Math.sin((pos.x % 1) * Math::PI) / 6
    zbounce = Math.sin((pos.z % 1) * Math::PI) / 6

    pos.y = 0.25 + xbounce + zbounce
    @camera.transform_component.position.z = 
      @camz + (xbounce + zbounce) / @transform.scale.z

    update_cursor_color
  end

  def update_cursor_color
    @cursor ||= find_entity("TileCursor")

    curpos = @cursor.transform_component.position
    dx = Math.round(curpos.x - @pos.x)
    dz = Math.round(curpos.z - @pos.y)

    if !can_move?(dx, dz)
      @cursor.children.first.sprite_component.texture_index = 1
    else
      @cursor.children.first.sprite_component.texture_index = 0
    end
  end

  def can_move?(xo, yo)
    @blocked_reason = -1

    if @logic_cooldown > 0
      @logic_cooldown -= GameTime.dt
      @blocked_reason = BLOCKED_BY_COOLDOWN
      return false
    end

    if xo != 0 and yo != 0
      @blocked_reason = BLOCKED_BY_UNKNOWN
      return false # unless can_move?(xo, 0) && can_move?(0, yo)
    end

    if Math.abs(xo) > 1.5 || Math.abs(yo) > 1.5
      @blocked_reason = BLOCKED_BY_UNKNOWN
      return false
    end

    room = current_floor

    # false if the move animation isn't done
    real_pos = @transform.position.dup
    real_pos.y = real_pos.z
    real_pos.z = @pos.z
    real_pos.w = 0

    @blocked_reason = BLOCKED_BY_UNKNOWN
    return false unless @pos.near? real_pos, 0.2

    x = (@pos.x + 0.5).to_i + xo
    y = (@pos.y + 0.5).to_i + yo

    @blocked_reason = BLOCKED_BY_UNKNOWN
    return false if x < 0 || x >= room[0].count
    return false if y < 0 || y >= room.count

    tile = room[room.count - 1 - y][x]
    @move_tile = tile

    res = !tile.actor?
    @blocked_reason = BLOCKED_BY_ACTOR
    return res unless res

    res = !tile.solid?
    @blocked_reason = BLOCKED_BY_WALL
    return res
  end

  def mouse_move(e)
    set_mouse_mode
  end

  def set_kb_mode
    return if Config[:touch_mode]
    @cursor ||= find_entity("TileCursor")
    @cursor.children[0].sprite_component.visible = false
  end

  def set_mouse_mode
    return if Config[:touch_mode]
    @cursor ||= find_entity("TileCursor")
    @cursor.children[0].sprite_component.visible = true
  end

  register_component "PlayerControllerComponent"
end
