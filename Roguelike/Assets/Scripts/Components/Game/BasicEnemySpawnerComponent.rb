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
    "Enemies/Wizard_Purple"
  ]

  serialized_input do |p|
    p.integer :num_enemies
  end

  def initialize(data)
    super data

    GENERATE_ENTITIES.lazy.select{|t| t[0] == 2}.each do |tuple|
      type, x, y, meta = tuple
      data = {}

      if meta.is_a? String
        meta = "Enemies/#{meta}"
      elsif meta.is_a? Array
        data = meta[1] || data
        meta = "Enemies/#{meta[0]}"
      else
        meta = ENEMY_TYPES.rand_item
      end

      Enemy.spawn_at(meta, x, y)
    end
  end

  def random_enemy
    ENEMY_TYPES.random_entry
  end

  register_component "BasicEnemySpawnerComponent"
end
