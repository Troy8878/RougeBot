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
  attr_reader :pos

  MIN_MOVE_TIME = 0.2

  # Initialize the properties of the PlayerController
  def initialize(data)
    super data

    puts self.owner.components.inspect

    @transform = self.owner.transform_component
    @pos = self.owner.position_component.position

    self.register_event :player_move, :on_move
    self.register_event :update, :first_update

    # Double-click should do it in either case
    # Double-click is the only way in touch mode
    self.register_event :double_click, :mouse_down
    unless Config[:touch_mode]
      self.register_event :mouse_down, :mouse_down
    end
  end

  def on_move(e)
    if can_move? *e
      move *e
    else
      puts @blocked_reason
    end
  end

  def mouse_down(e)
    @cursor ||= find_entity("TileCursor")

    curpos = @cursor.transform_component.position
    dx = Math.round(curpos.x - @pos.x)
    dy = Math.round(curpos.z - @pos.y)

    return if Math.abs(dx) > 1.5 || Math.abs(dy) > 1.5

    if can_move? dx, dy
      move dx, dy
    else
      puts @blocked_reason
    end
  end

  def create_mapitem
    return

    # Create a MapItem.
    @minimap ||= find_entity("Minimap")
    @map_item = @minimap.map_component.create_item
    @map_item.shape = MapItem::ELLIPSE
    @map_item.color = "Yellow"
  end

  def update_mapitem
    return

    # Update the position on the map
    @map_item.x = @pos.x
    @map_item.y = @pos.y
    @minimap.raise_event :map_update, nil
  end

  def move(x, y)
    @pos.x += x
    @pos.y += y
    update_mapitem
  end

  def first_update(e)
    create_mapitem

    move 0, 0

    @camera = find_entity("CameraRoot")
    @camera.parent = self.owner
    @camz = @camera.transform_component.position.z

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
      @blocked_reason = "Can't move diagonally"
      return false # unless can_move?(xo, 0) && can_move?(0, yo)
    end

    if Math.abs(xo) > 1.5 || Math.abs(yo) > 1.5
      @blocked_reason = "Can't move more than 1 space at a time"
      return false
    end

    room = current_floor

    # false if the move animation isn't done
    real_pos = @transform.position.dup
    real_pos.y = real_pos.z
    real_pos.z = @pos.z
    real_pos.w = 0

    @blocked_reason = "Already moving"
    return false unless @pos.near? real_pos, 0.2

    x = (@pos.x + 0.5).to_i + xo
    y = (@pos.y + 0.5).to_i + yo

    @blocked_reason = "Out of bounds"
    return false if x < 0 || x >= room[0].count
    return false if y < 0 || y >= room.count

    tile = room[room.count - 1 - y][x]

    res = !tile.actor?
    @blocked_reason = "Blocked by actor"
    return res unless res

    res = !tile.solid?
    @blocked_reason = "Blocked by wall"
    return res
  end

  register_component "PlayerControllerComponent"
end
