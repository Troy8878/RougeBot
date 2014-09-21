#######################
# TestRoomComponent.rb
# Connor Hilarides
# Created 2014/09/09
#######################

class TestRoomComponent < ComponentBase
  include ModelBuilder

  attr_accessor :room

  EMPTY_VALUE = 0
  WALL_VALUE = 1

  ROOM_SIZE = 50
  C_L = -0.5
  C_R = 0.5
  W_T = 0.5
  W_B = 0

  WHITE = Vector.new(1, 1, 1, 1)

  TE = 0.001

  CEIL_TL  = Vector.new(0, 0       )
  CEIL_TR  = Vector.new(1, 0       )
  CEIL_BR  = Vector.new(1, 1/3 - TE)
  CEIL_BL  = Vector.new(0, 1/3 - TE)

  WALL_TL  = Vector.new(0, 1/3 + TE)
  WALL_TR  = Vector.new(1, 1/3 + TE)
  WALL_BR  = Vector.new(1, 1/2 - TE)
  WALL_BL  = Vector.new(0, 1/2 - TE)

  FLOOR_TL = Vector.new(0, 2/3 + TE)
  FLOOR_TR = Vector.new(1, 2/3 + TE)
  FLOOR_BR = Vector.new(1, 1       )
  FLOOR_BL = Vector.new(0, 1       )

  def initialize(data)
    super data

    generate_room

    register_event :update, :first_update
  end

  def first_update(e)
    ent = find_entity("Pancake")

    # Set the movement target
    pcc = ent.player_controller_component
    ppos = pcc.instance_variable_get :@pos
    ppos.x = @px
    ppos.z = @py

    # Start the actual position at the beginning
    trc = ent.transform_component
    tpos = trc.position
    tpos.x = @px
    tpos.z = @py

    # Start the camera there too
    cam = find_entity("CameraRoot")
    cfc = cam.camera_follow_component
    cfr = cfc.instance_variable_get :@follow_real
    unless cfr.nil?
      cfr = tpos.dup
      cfc.instance_variable_set :@follow_real, cfr
    end

    remove_event :update
  end

  private
  def generate_room
    @room, @px, @py = Floor.generate(
      width: ROOM_SIZE, height: ROOM_SIZE, 
      birth: 3, death: 4,
      steps: 1
    )

    build_model do |builder|
      (-7..(ROOM_SIZE + 6)).combinations do |y, x|
        if y < 0 || x < 0 || y >= ROOM_SIZE || x >= ROOM_SIZE
          add_ceil(builder, x, y)
          next
        end

        v = room_get(x, y)
        if v == WALL_VALUE
          add_ceil(builder, x, y)
        else
          add_floor(builder, x, y)

          if room_get(x - 1, y) == WALL_VALUE
            add_west_wall(builder, x, y)
          end
          if room_get(x + 1, y) == WALL_VALUE
            add_east_wall(builder, x, y)
          end
          if room_get(x, y - 1) == WALL_VALUE
            add_north_wall(builder, x, y)
          end
          if room_get(x, y + 1) == WALL_VALUE
            add_south_wall(builder, x, y)
          end
        end
      end

      y = -0.1
      l_x = -7.5
      b_z = -7.5
      r_x = ROOM_SIZE + 7.5
      t_z = ROOM_SIZE + 7.5

      builder.add_quad(
        Vertex.new(Vector.new(l_x, y, b_z, 1), WHITE, CEIL_BL),
        Vertex.new(Vector.new(l_x, y, t_z, 1), WHITE, CEIL_TL),
        Vertex.new(Vector.new(r_x, y, t_z, 1), WHITE, CEIL_TR),
        Vertex.new(Vector.new(r_x, y, b_z, 1), WHITE, CEIL_BR)
      )
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

    return WALL_VALUE if y < 0 or y >= ROOM_SIZE
    return WALL_VALUE if x < 0 or x >= ROOM_SIZE

    @room[y][x]
  end

  register_component "TestRoomComponent"
end
