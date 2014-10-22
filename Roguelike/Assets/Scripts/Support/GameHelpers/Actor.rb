#########################
# Actor.rb
# Connor Hilarides
# Created 2014/10/20
#########################

module Actor
  attr_reader :actor_position

  def actor_init(shape, color)
    actor_minimap_create shape, color

    @actor_position = self.owner.position_component.position

    register_event :zombified, :actor_zombified
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
  end

  def actor_minimap_refresh
    @actor_minimap.local_event :map_update, nil
  end

  def actor_minimap_delete
    @actor_minimap.map_component.delete_item @actor_map_item
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
end
