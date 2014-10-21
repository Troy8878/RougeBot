#########################
# Actor.rb
# Connor Hilarides
# Created 2014/10/20
#########################

module Actor
  def actor_position
    self.owner.position_component.position
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
  end

  def current_tile
    pos = actor_position
    room = current_floor
    room[room.size - 1 - pos.y][pos.x]
  end
end
