#########################
# RandomEnemySpawner.rb
# Connor Hilarides
# Created 2014/10/24
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

module Enemy
  def self.find_empty_slot(floor)
    # Try to find a random one
    100.times do
      x = Random.int_range(0, floor[0].length)
      y = Random.int_range(0, floor.length)

      tile = floor[y][x]

      return tile unless tile.solid? || tile.actor?
    end

    # If we didn't find one randomly, screw it and search the level
    floor.each do |row|
      row.each do |tile|
        return tile unless tile.solid? || tile.actor?
      end
    end

    # The world is full D:
    return floor[0][0]
  end

  def self.spawn(type)
    floor = current_floor

    slot = Enemy.find_empty_slot floor
    return if slot.nil?

    spawn_in(type, slot)
  end

  def self.spawn_in(type, slot)
    floor = current_floor
    enemy = find_entity(0).create_child(
      archetype: type,
      components: {
        "PositionComponent" => {
          "position" => [slot.x, floor.length - 1 - slot.y]
        }
      }
    )

    enemy.defense_component.health *= GAME_STATE[:floor]
    enemy
  end

  def self.spawn_at(type, x, y)
    floor = current_floor
    slot = floor[y][x]
    spawn_in(type, slot)
  end
end
