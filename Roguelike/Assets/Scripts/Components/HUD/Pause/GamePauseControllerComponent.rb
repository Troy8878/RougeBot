#######################
# GamePauseControllerComponent.rb
# Connor Hilarides
# Created 2014/11/14
#######################

class GamePauseControllerComponent < ComponentBase
  property :paused, :bool, true

  def initialize(data)
    super data

    @paused = false

    register_event :pause, :on_pause
    register_event :update, :first_update
    register_event :is_paused, :is_paused
  end

  def first_update(e)
    remove_event :update
  end

  def on_pause(e)
    if paused?
      self.paused = false
    else
      self.paused = true
    end
  end

  def is_paused(e)
    e[:paused] = paused?
  end

  def paused
    @paused
  end

  alias paused? paused

  def paused=(val)
    return if @paused == val
    @paused = val

    Event.raise_event :on_pause, val

    if paused?
      find_entity("HUD_ROOT").transform_component.position.y = -10
      KeybindingComponent.lock!
    else
      find_entity("HUD_ROOT").transform_component.position.y = 0
      KeybindingComponent.unlock!
    end
  end

  register_component
end
