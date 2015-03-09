#########################
# DebugHelp.rb
# Connor Hilarides
# Created 2014/09/07
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

def find_entity(*args)
  CURRENT_LEVEL.root_entity.find_entity(*args)
end

def search_entities(*args)
  CURRENT_LEVEL.root_entity.search_entities(*args)
end

def start_debug
  PANCAKE = find_entity("Pancake")
  POS = PANCAKE.transform_component.position

  def blocked?(xo = 0, yo = 0)
    room = TestRoomComponent.class_variable_get("@@room")

    x = (POS.x + 0.5).to_i + xo
    y = (POS.z + 0.5).to_i + yo
    room[room.count - 1 - y][x] == 1
  end

  def up
    return :nope if blocked?(0, 1)
    POS.z += 1
  end

  def down
    return :nope if blocked?(0, -1)
    POS.z -= 1
  end

  def right
    return :nope if blocked?(1, 0)
    POS.x += 1
  end

  def left
    return :nope if blocked?(-1, 0)
    POS.x -= 1
  end

  def whereami
    "x: #{(POS.x + 0.5).to_i}, y: #{(POS.z + 0.5).to_i}"
  end
end
