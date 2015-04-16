#########################
# BasicEnemySpawnerComponent.rb
# Claire Robsahm
# Created 2014/10/20
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

class BasicEnemySpawnerComponent < ComponentBase

  ENEMY_TYPES = [
    "Enemies/Slime",
    "Enemies/Wizard_Red",
    "Enemies/Wizard_Orange",
    "Enemies/Wizard_Yellow",
    "Enemies/Wizard_Green",
    "Enemies/Wizard_Blue",
    "Enemies/Wizard_Purple",
    "Enemies/Wizard_Angst",
    "Enemies/Derpaderp"
  ]

  serialized_input do |p|
    p.integer :num_enemies
  end

  def initialize(data)
    super data

    GENERATE_ENTITIES.each do |tuple|
      type, x, y, meta = tuple
      data = {}

      case type
      when 2 # Enemy
        if meta.is_a? String
          meta = "Enemies/#{meta}"
        elsif meta.is_a? Array
          data = meta[1] || data
          meta = "Enemies/#{meta[0]}"
        else
          meta = ENEMY_TYPES.rand_item
        end
      when 3 # Breakable wall
        meta = "BorkWall"
      when 5 # Item spawn
        if meta.is_a? Float
          meta = meta.to_i
        else
          meta = GAME_STATE[:floor]
        end

        tile = current_floor[y][x]
        tile.drop_item ItemGenerate.generate_weapon({}, meta)
        next
      else
        next
      end

      Enemy.spawn_at(meta, x, y)
    end
  end

  def random_enemy
    ENEMY_TYPES.random_entry
  end

  register_component "BasicEnemySpawnerComponent"
end
