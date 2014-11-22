#########################
# BasicEnemySpawnerComponent.rb
# Jake Robsahm
# Created 2014/10/20
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

class BasicEnemySpawnerComponent < ComponentBase

  ENEMY_TYPES = [
    "Enemies/Slime",
    "Enemies/Derpaderp"
  ]

  def initialize(data)
    super data

    @num_enemies = data.fetch("num_enemies", 8).to_i

    register_event :update, :first_update
  end

  def random_enemy
    ENEMY_TYPES.random_entry
  end

  def first_update(e)
    @num_enemies.times do
      Enemy.spawn random_enemy
    end

    owner.remove_component "BasicEnemySpawnerComponent"
  end

  register_component "BasicEnemySpawnerComponent"
end
