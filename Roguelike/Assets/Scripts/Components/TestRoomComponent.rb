#######################
# TestRoomComponent.rb
# Connor Hilarides
# Created 2014/09/09
#######################

class TestRoomComponent < ComponentBase
  include ModelBuilder

  @@room = [
    [1, 1, 0, 0, 0, 1, 1],
    [1, 1, 0, 1, 0, 1, 1],
    [0, 0, 0, 0, 0, 0, 0],
    [0, 1, 0, 1, 0, 1, 0],
    [0, 0, 0, 1, 0, 0, 0],
    [1, 1, 0, 1, 0, 1, 1],
    [1, 1, 0, 0, 0, 1, 1],
  ]

  @@grey = Vector.new(0.1, 0.1, 0.1, 1)
  @@blue = Vector.new(0.1, 0.1, 1, 1)
  @@white = Vector.new(1, 1, 1, 1)

  def initialize(data)
    super data

    generate_room
  end

  private
  def generate_room
    build_model do |builder|
      (0..6).combinations do |y, x|
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
      Vertex.new(Vector.new(x - 3.5, 0.25, y - 3.5, 1), @@grey),
      Vertex.new(Vector.new(x - 2.5, 0.25, y - 3.5, 1), @@grey),
      Vertex.new(Vector.new(x - 2.5, 0.25, y - 2.5, 1), @@grey),
      Vertex.new(Vector.new(x - 3.5, 0.25, y - 2.5, 1), @@grey)
    )
  end

  private
  def add_floor(builder, x, y)
    builder.add_quad(
      Vertex.new(Vector.new(x - 3.5, 0, y - 3.5, 1), @@white),
      Vertex.new(Vector.new(x - 2.5, 0, y - 3.5, 1), @@white),
      Vertex.new(Vector.new(x - 2.5, 0, y - 2.5, 1), @@white),
      Vertex.new(Vector.new(x - 3.5, 0, y - 2.5, 1), @@white)
    )
  end

  private
  def add_west_wall(builder, x, y)
    builder.add_quad(
      Vertex.new(Vector.new(x - 3.5, 0.25, y - 2.5, 1), @@blue),
      Vertex.new(Vector.new(x - 3.5, 0.25, y - 3.5, 1), @@blue),
      Vertex.new(Vector.new(x - 3.5, 0, y - 3.5, 1), @@blue),
      Vertex.new(Vector.new(x - 3.5, 0, y - 2.5, 1), @@blue)
    )
  end

  private
  def add_east_wall(builder, x, y)
    builder.add_quad(
      Vertex.new(Vector.new(x - 2.5, 0.25, y - 2.5, 1), @@blue),
      Vertex.new(Vector.new(x - 2.5, 0.25, y - 3.5, 1), @@blue),
      Vertex.new(Vector.new(x - 2.5, 0, y - 3.5, 1), @@blue),
      Vertex.new(Vector.new(x - 2.5, 0, y - 2.5, 1), @@blue)
    )
  end

  private
  def add_north_wall(builder, x, y)
    builder.add_quad(
      Vertex.new(Vector.new(x - 2.5, 0.25, y - 3.5, 1), @@blue),
      Vertex.new(Vector.new(x - 3.5, 0.25, y - 3.5, 1), @@blue),
      Vertex.new(Vector.new(x - 3.5, 0, y - 3.5, 1), @@blue),
      Vertex.new(Vector.new(x - 2.5, 0, y - 3.5, 1), @@blue)
    )
  end

  private
  def add_south_wall(builder, x, y)
    builder.add_quad(
      Vertex.new(Vector.new(x - 2.5, 0.25, y - 2.5, 1), @@blue),
      Vertex.new(Vector.new(x - 3.5, 0.25, y - 2.5, 1), @@blue),
      Vertex.new(Vector.new(x - 3.5, 0, y - 2.5, 1), @@blue),
      Vertex.new(Vector.new(x - 2.5, 0, y - 2.5, 1), @@blue)
    )
  end

  private
  def room_get(x, y)
    y = 6 - y # because muh upside down

    return 1 if x < 0 or y < 0 or y >= @@room.count
    row = @@room[y]
    return 1 if x >= row.count

    row[x]
  end

  register_component "TestRoomComponent"
end
