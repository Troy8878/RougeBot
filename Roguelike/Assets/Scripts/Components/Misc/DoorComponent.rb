#######################
# DoorComponent.rb
# Connor Hilarides
# Created 2015/04/13
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class DoorComponent < ComponentBase
  attr_reader :tutnum

  serialized_input do |p|
    p.integer :tutnum, default: -2
  end

  def initialize(data)
    super data

    @tutnum = data["tutnum"]
    if @tutnum == -2
      @tutnum = nil
    end

    register_event :update, :first_update
  end

  def first_update(e)
    remove_event :update

    pos = owner.position_component.position
    room = current_floor
    room[room.size - pos.y - 1][pos.x].door = self
  end

  register_component
end

