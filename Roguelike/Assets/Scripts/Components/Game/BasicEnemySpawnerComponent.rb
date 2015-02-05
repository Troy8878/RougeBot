#########################
# BasicEnemySpawnerComponent.rb
# Claire Robsahm
# Created 2014/10/20
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

class BasicEnemySpawnerComponent < ComponentBase

  ENEMY_TYPES = [
    "Enemies/Slime",
    "Enemies/Derpaderp",
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

    num_enemies = data.fetch("num_enemies", 8).to_i

    # Delay the spawning by 2 frames
    seq = owner.action_sequence :spawn
    seq.delay 0
    seq.delay 0
    seq.once do
      num_enemies.times do
        Enemy.spawn random_enemy
      end
    end
  end

  def random_enemy
    ENEMY_TYPES.random_entry
  end

  register_component "BasicEnemySpawnerComponent"
end
