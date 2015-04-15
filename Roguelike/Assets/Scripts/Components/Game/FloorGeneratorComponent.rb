#######################
# FloorGeneratorComponent.rb
# Connor Hilarides
# Created 2014/09/09
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

$DungeonLength = 5

def current_floor
  FloorGeneratorComponent.instance.floor
end

class FloorGeneratorComponent < ComponentBase
  attr_accessor :floor

  serialized_input do |p|
  end

  def initialize(data)
    super data

    @@instance = self

    generate_room

    register_event :update, :first_update
  end

  def first_update(e)
    if GAME_STATE[:tutorial].is_a? Fixnum
      if GAME_STATE[:tutorial] > -1
        owner.local_event :set_model_tex, 3
      end
    else
      id = 1
      if GAME_STATE[:floor] > 1
        id = Random.die_roll(2)
      end
      owner.local_event :set_model_tex, id
    end

    # Set the movement target
    ent = find_entity("Player")
    pos = ent.position_component
    pos.position.x = floor.player_start[0]
    pos.position.y = floor.player_start[1]
    pos.jump(0)

    remove_event :update
  end

  def self.instance
    @@instance
  end

  register_component "FloorGeneratorComponent"
end

def current_floor
  FloorGeneratorComponent.instance.floor
end
