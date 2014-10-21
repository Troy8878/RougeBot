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

  # Initialize the properties of the PlayerController
  def initialize(data)
    super data

    puts self.owner.components.inspect

    @transform = self.owner.transform_component
    @pos = self.owner.position_component.position

    @logic_initialized = false

    self.register_event :player_move, :on_move
    self.register_event :update, :first_update

    # Double-click should do it in either case
    # Double-click is the only way in touch mode
    self.register_event :double_click, :mouse_down
    unless Config[:touch_mode]
      self.register_event :mouse_down, :mouse_down
    end
  end

  def move(x, y)
    unless can_move? x, y
      if @blocked_reason != BLOCKED_BY_ACTOR
        return
      end
    end

    if @blocked_reason == BLOCKED_BY_ACTOR
      self.owner.attack_component.do_attack @move_tile.actor
    else
      @pos.x += x
      @pos.y += y

      update_mapitem
      actor_moved
    end

    if @logic_initialized
      Event.raise_event :logic_update, self.owner
    end
  end

  def on_move(e)
    move *e
  end

  def mouse_down(e)
    @cursor ||= find_entity("TileCursor")
    @curpos ||= @cursor.transform_component.position

    dx = Math.round(@curpos.x - @pos.x)
    dy = Math.round(@curpos.z - @pos.y)

    move dx, dy
  end

  def create_mapitem
    # Create a MapItem.
    @minimap ||= find_entity("Minimap")

    return if @minimap.nil? || @minimap.map_component.nil?

    @map_item = @minimap.map_component.create_item
    @map_item.shape = MapItem::ELLIPSE
    @map_item.color = "Yellow"
  end

  def update_mapitem
    return if @minimap.nil? || @minimap.map_component.nil?

    # Update the position on the map
    @map_item.x = @pos.x
    @map_item.y = @pos.y
    @minimap.local_event :map_update, nil
  end

  def first_update(e)
    create_mapitem

    move 0, 0

    @camera = find_entity("CameraRoot")
    @camera.parent = self.owner
    @camz = @camera.transform_component.position.z

    @logic_initialized = true

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

  register_component "PlayerControllerComponent"
end
