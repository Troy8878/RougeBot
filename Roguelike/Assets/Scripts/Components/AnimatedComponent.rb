#######################
# AnimatedComponent.rb
# Connor Hilarides
# Created 2014/09/01
#######################

#########################Class Details############################
# This class is the component that animates an object with a
# sprite component.
#######################Property Details###########################
# paused - starts the animation component in a paused state (bool)
# frame_time - number of seconds each frame is viewed for
# frame_range - begin and end frame index.
##################################################################

class AnimatedComponent < ComponentBase
  attr_accessor :paused, :frame_time, :frame_range, :time

  def initialize(data)
    super data

    @sprite = self.owner.sprite_component
    @paused = data.fetch("paused", "false") == "true"
    @frame_time = data.fetch("frame_time", 1 / 24).to_f
    @frame_range = data.fetch("frame_range", [0, 0xffff]).map{|f| f.to_i }
    @time = 0

    self.register_event :update, :animate
  end

  ##
  # :update event handler
  # handles animating the frames and keeping
  # track of the time (i.e. when to change)
  def animate(e)
    # don't even try when paused
    return if @paused

    # add the time to keep track of when to change
    @time += e.dt

    # increment frames. `while` in case FPS is lower than frame_time
    while @time >= @frame_time
      @time -= @frame_time
      next_frame
    end
  end

  ##
  # Sets the owner's sprite to the next frame
  def next_frame
    i = @sprite.texture_index + 1
    @sprite.texture_index = wrap_index i
  end

  ##
  # Wraps i to be within frame_range 
  # and texture_count bounds
  def wrap_index(i)
    if i >= @sprite.texture_count || i > @frame_range[1]
      i = @frame_range[0]
    end
    return i
  end

  register_component "AnimatedComponent"
end
