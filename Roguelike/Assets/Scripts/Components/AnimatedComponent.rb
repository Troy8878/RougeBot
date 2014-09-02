#######################
# AnimatedComponent.rb
# Connor Hilarides
# Created 2014/09/01

class AnimatedComponent < ComponentBase
  def initialize(data)
    super data

    @sprite = self.owner.get_component "SpriteComponent"
    @frame_time = data.fetch("frameTime", 1 / 24).to_f
    @time = 0

    self.register_event :update, :animate
  end

  def animate(e)
    @time += e.dt

    if @time > @frame_time
      @time -= frame_time
      next_frame
    end
  end

  def next_frame
    # TODO: Animate the frame
    # I still neet to make a ruby wrapper for SpriteComponent
  end

  register_component "AnimatedComponent"
end
