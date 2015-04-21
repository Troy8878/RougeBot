#######################
# FloorGeneratorComponentModelGeneration.rb
# Connor Hilarides
# Created 2014/09/09
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#######################

class FloorGeneratorComponent < ComponentBase
  include TilemapBuilder

  EMPTY_VALUE = 0
  WALL_VALUE = 1
  ROOM_SIZE = 50
  TEXTURE_COUNT = 3

  TEX_WALL = 0
  TEX_FLOOR = 2

  private
  def generate_room
    @floor = Map.new
    @floor.generate(
      width: ROOM_SIZE, height: ROOM_SIZE
    )

    build_model(TEXTURE_COUNT) do |builder|
      (-7..(ROOM_SIZE + 6)).combinations do |y, x|
        if y < 0 || x < 0 || y >= ROOM_SIZE || x >= ROOM_SIZE
          builder.add_tile(x, y, TEX_WALL)
          next
        end

        v = room_get(x, y)
        if v == WALL_VALUE
          builder.add_tile(x, y, TEX_WALL)
        else
          builder.add_tile(x, y, TEX_FLOOR)
        end
      end
    end
  end

  private
  def room_get(x, y)
    y = ROOM_SIZE - 1 - y # because muh upside down

    return WALL_VALUE if y < 0 or y >= ROOM_SIZE
    return WALL_VALUE if x < 0 or x >= ROOM_SIZE

    @floor[y][x].type_id
  end
end
