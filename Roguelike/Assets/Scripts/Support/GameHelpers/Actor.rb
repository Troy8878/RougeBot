#########################
# Actor.rb
# Troy
# Created 2014/10/20
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

module Actor
  attr_reader :actor_position

  module_dependency "PositionComponent", "TransformComponent"

  NOT_BLOCKED = 0
  BLOCKED_BY_UNKNOWN = 1
  BLOCKED_BY_ACTOR = 2
  BLOCKED_BY_WALL = 3
  BLOCKED_BY_COOLDOWN = 4
  CANNOT_MOVE_DIAGONALLY = 5

  def ghost_actor!
    @ghost_actor = true
    actor_removed
  end

  def actor_init(shape, color)
    actor_minimap_create shape, color

    @actor_transform = self.owner.transform_component
    @actor_position = self.owner.position_component.position
    @actor_diagonal = false

    register_event :zombified, :actor_zombified
  end

  def actor_sub_init
    @actor_transform = self.owner.transform_component
    @actor_position = self.owner.position_component.position
    @actor_diagonal = false
  end

  def actor_init?
    !@actor_position.nil?
  end

  def actor_minimap_create(shape, color)
    @actor_minimap = find_entity("Minimap")

    @actor_map_item = @actor_minimap.map_component.create_item
    @actor_map_item.shape = shape
    @actor_map_item.color = color
  end

  def actor_minimap_update
    @actor_map_item.x = actor_position.x
    @actor_map_item.y = actor_position.y
    actor_minimap_refresh
  end

  def actor_minimap_refresh
    @actor_minimap.local_event :map_update, nil
  end

  def actor_minimap_delete
    @actor_minimap.map_component.delete_item @actor_map_item
    actor_minimap_refresh
  end

  def actor_zombified(e)
    @prev_tile.actor = nil if @prev_tile
    actor_minimap_delete
  end

  def actor_removed
    if @prev_tile
      @prev_tile.actor = nil
    end
  end

  def actor_moved
    actor_removed

    return if @ghost_actor

    new_tile = current_tile
    new_tile.actor = self.owner
    @prev_tile = new_tile

    actor_minimap_update
  end

  def current_tile
    pos = actor_position
    room = current_floor
    room[room.size - 1 - pos.y][pos.x]
  end

  def can_move?(xo, yo)
    @blocked_reason = -1

    if !@logic_cooldown.nil? && @logic_cooldown > 0
      @logic_cooldown -= GameTime.dt
      @blocked_reason = BLOCKED_BY_COOLDOWN
      return false
    end

    if xo != 0 && yo != 0
      unless @actor_diagonal
        @blocked_reason = CANNOT_MOVE_DIAGONALLY
        return false
      end
    end

    if Math.abs(xo) > 1.5 || Math.abs(yo) > 1.5
      @blocked_reason = BLOCKED_BY_UNKNOWN
      return false
    end

    room = current_floor

    # false if the move animation isn't done
    real_pos = @actor_transform.position.dup
    real_pos.z = 0
    real_pos.w = 0

    @blocked_reason = BLOCKED_BY_UNKNOWN
    return false unless @actor_position.near? real_pos, 0.2

    x = (@actor_position.x + 0.5).to_i + xo
    y = (@actor_position.y + 0.5).to_i + yo

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

# Ranged combat below here

  def can_move_more_than_one?(xo, yo)
    @blocked_reason = -1

    if !@logic_cooldown.nil? && @logic_cooldown > 0
      @logic_cooldown -= GameTime.dt
      @blocked_reason = BLOCKED_BY_COOLDOWN
      return false
    end

    if xo != 0 and yo != 0
      unless @actor_diagonal
        @blocked_reason = CANNOT_MOVE_DIAGONALLY
        return false
      end
    end

    if Math.abs(xo) > 1.5 || Math.abs(yo) > 1.5
      @blocked_reason = BLOCKED_BY_UNKNOWN
      return false
    end

    room = current_floor

    # false if the move animation isn't done
    real_pos = @actor_transform.position.dup
    real_pos.z = 0
    real_pos.w = 0

    x = (@actor_position.x + 0.5).to_i + xo
    y = (@actor_position.y + 0.5).to_i + yo

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

    @blocked_reason = BLOCKED_BY_UNKNOWN
    return false unless @actor_position.near? real_pos, 0.2
  end

  def can_fire?(xo, yo) # Very much the same as can_move right now.  can_move will change, this won't
    @blocked_reason = -1

    if !@logic_cooldown.nil? && @logic_cooldown > 0
      @logic_cooldown -= GameTime.dt
      @blocked_reason = BLOCKED_BY_COOLDOWN
      return false
    end

    if Math.abs(xo) > 1.5 || Math.abs(yo) > 1.5
      @blocked_reason = BLOCKED_BY_UNKNOWN
      return false
    end

    room = current_floor

    # false if the move animation isn't done
    real_pos = @actor_transform.position.dup
    real_pos.z = 0
    real_pos.w = 0

    @blocked_reason = BLOCKED_BY_UNKNOWN
    return false unless @actor_position.near? real_pos, 0.2

    x = (@actor_position.x + 0.5).to_i + xo
    y = (@actor_position.y + 0.5).to_i + yo

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
end
