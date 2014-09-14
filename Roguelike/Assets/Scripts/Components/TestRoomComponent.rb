#######################
# TestRoomComponent.rb
# Connor Hilarides
# Created 2014/09/09
#######################

class TestRoomComponent < ComponentBase
  include ModelBuilder

  @@room = [
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
    [1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    [1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1],
    [1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1],
    [1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1],
    [1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1],
    [1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1],
    [1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1],
    [1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1],
    [1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1],
    [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
  ]

  ROOM_SIZE = @@room.count
  C_L = -0.5
  C_R = 0.5
  W_T = 0.5
  W_B = 0

  WHITE = Vector.new(1, 1, 1, 1)

  CEIL_TL = Vector.new(0, 0  )
  CEIL_TR = Vector.new(1, 0  )
  CEIL_BL = Vector.new(0, 1/3)
  CEIL_BR = Vector.new(1, 1/3)

  WALL_TL = Vector.new(0, 1/3)
  WALL_TR = Vector.new(1, 1/3)
  WALL_BL = Vector.new(0, 1/2)
  WALL_BR = Vector.new(1, 1/2)

  FLOOR_TL = Vector.new(0, 2/3)
  FLOOR_TR = Vector.new(1, 2/3)
  FLOOR_BL = Vector.new(0, 1  )
  FLOOR_BR = Vector.new(1, 1  )

  def initialize(data)
    super data

    generate_room
  end

  private
  def generate_room
    build_model do |builder|
      (0..(ROOM_SIZE - 1)).combinations do |y, x|
        v = room_get(x, y)
        if v == 1
          add_ceil(builder, x, y)
        else
          add_floor(builder, x, y)

          if room_get(x - 1, y) == 1
            add_west_wall(builder, x, y)
          end
          if room_get(x + 1, y) == 1
            add_east_wall(builder, x, y)
          end
          if room_get(x, y - 1) == 1
            add_north_wall(builder, x, y)
          end
          if room_get(x, y + 1) == 1
            add_south_wall(builder, x, y)
          end
        end
      end
    end
  end

  private
  def add_ceil(builder, x, y)
    builder.add_quad(
      Vertex.new(Vector.new(x + C_L, W_T, y + C_L, 1), WHITE, CEIL_TL),
      Vertex.new(Vector.new(x + C_R, W_T, y + C_L, 1), WHITE, CEIL_TR),
      Vertex.new(Vector.new(x + C_R, W_T, y + C_R, 1), WHITE, CEIL_BR),
      Vertex.new(Vector.new(x + C_L, W_T, y + C_R, 1), WHITE, CEIL_BL)
    )
  end

  private
  def add_floor(builder, x, y)
    builder.add_quad(
      Vertex.new(Vector.new(x + C_L, W_B, y + C_L, 1), WHITE, FLOOR_TL),
      Vertex.new(Vector.new(x + C_R, W_B, y + C_L, 1), WHITE, FLOOR_TR),
      Vertex.new(Vector.new(x + C_R, W_B, y + C_R, 1), WHITE, FLOOR_BR),
      Vertex.new(Vector.new(x + C_L, W_B, y + C_R, 1), WHITE, FLOOR_BL)
    )
  end

  private
  def add_west_wall(builder, x, y)
    builder.add_quad(
      Vertex.new(Vector.new(x + C_L, W_T, y + C_R, 1), WHITE, WALL_TL),
      Vertex.new(Vector.new(x + C_L, W_T, y + C_L, 1), WHITE, WALL_TR),
      Vertex.new(Vector.new(x + C_L, W_B, y + C_L, 1), WHITE, WALL_BR),
      Vertex.new(Vector.new(x + C_L, W_B, y + C_R, 1), WHITE, WALL_BL)
    )
  end

  private
  def add_east_wall(builder, x, y)
    builder.add_quad(
      Vertex.new(Vector.new(x + C_R, W_T, y + C_R, 1), WHITE, WALL_TL),
      Vertex.new(Vector.new(x + C_R, W_T, y + C_L, 1), WHITE, WALL_TR),
      Vertex.new(Vector.new(x + C_R, W_B, y + C_L, 1), WHITE, WALL_BR),
      Vertex.new(Vector.new(x + C_R, W_B, y + C_R, 1), WHITE, WALL_BL)
    )
  end

  private
  def add_north_wall(builder, x, y)
    builder.add_quad(
      Vertex.new(Vector.new(x + C_R, W_T, y + C_L, 1), WHITE, WALL_TL),
      Vertex.new(Vector.new(x + C_L, W_T, y + C_L, 1), WHITE, WALL_TR),
      Vertex.new(Vector.new(x + C_L, W_B, y + C_L, 1), WHITE, WALL_BR),
      Vertex.new(Vector.new(x + C_R, W_B, y + C_L, 1), WHITE, WALL_BL)
    )
  end

  private
  def add_south_wall(builder, x, y)
    builder.add_quad(
      Vertex.new(Vector.new(x + C_R, W_T, y + C_R, 1), WHITE, WALL_TL),
      Vertex.new(Vector.new(x + C_L, W_T, y + C_R, 1), WHITE, WALL_TR),
      Vertex.new(Vector.new(x + C_L, W_B, y + C_R, 1), WHITE, WALL_BR),
      Vertex.new(Vector.new(x + C_R, W_B, y + C_R, 1), WHITE, WALL_BL)
    )
  end

  private
  def room_get(x, y)
    y = ROOM_SIZE - 1 - y # because muh upside down

    return 1 if x < 0 or y < 0 or y >= ROOM_SIZE
    row = @@room[y]
    return 1 if x >= row.count

    row[x]
  end

  register_component "TestRoomComponent"
end
