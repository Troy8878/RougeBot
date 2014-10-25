#######################
# FloorGeneratorComponent.rb
# Connor Hilarides
# Created 2014/09/09
#######################

class FloorGeneratorComponent < ComponentBase
  attr_accessor :floor

  def initialize(data)
    super data

    @@instance = self

    generate_room

    register_event :update, :first_update
  end

  def first_update(e)
    ent = find_entity("Player")

    # Set the movement target
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
