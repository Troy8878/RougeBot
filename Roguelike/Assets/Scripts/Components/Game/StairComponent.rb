#######################
# StairComponent.rb
# Claire Robsahm
# Created 2014/12/04
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#######################

STAIR_POSITION = Vector.new

class StairComponent < ComponentBase
  dependency "PositionComponent"

  serialized_input do |p|
    p.dependency "PositionComponent"
  end

  def initialize(data)
    super data

    register_event :update, :build_it
  end

  def build_it(e)
    owner.position_component.position = STAIR_POSITION

    put_on_minimap

    remove_event :update
  end

  def put_on_minimap
    @actor_minimap = find_entity("Minimap")

    @actor_map_item = @actor_minimap.map_component.create_item
    @actor_map_item.shape = MapItem::ELLIPSE
    @actor_map_item.color = "Purple, 0.9"
    @actor_map_item.x = STAIR_POSITION.x
    @actor_map_item.y = STAIR_POSITION.y
    @actor_map_item.stairs = true
    @actor_minimap.local_event :map_update, nil
  end

  register_component
end
