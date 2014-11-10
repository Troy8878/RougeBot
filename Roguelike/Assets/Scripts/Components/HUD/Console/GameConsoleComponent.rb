###########################
# GameConsoleComponent.rb
# Connor Hilarides
# Created 2014/10/24
###########################

class GameConsoleComponent < ComponentBase
  dependency "TransformComponent"

  OPEN_Y = 0.2
  CLOSE_Y = 3

  OPEN_SPEED = 5
  CLOSE_SPEED = 8

  def initialize(data)
    super data

    @open = false
    @pos = owner.transform_component.position

    register_event :update, :on_update
    register_event :toggle_console, :on_toggle
  end

  def on_update(e)
    if @open
      @pos.y -= e.dt * OPEN_SPEED
      if @pos.y < OPEN_Y
        @pos.y = OPEN_Y
      end
    else
      @pos.y += e.dt * CLOSE_SPEED
      if @pos.y > CLOSE_Y
        @pos.y = CLOSE_Y
      end
    end
  end

  register_component "GameConsoleComponent"
end
