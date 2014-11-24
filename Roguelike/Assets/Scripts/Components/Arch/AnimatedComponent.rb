#######################
# AnimatedComponent.rb
# Connor Hilarides
# Created 2014/09/01
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
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

  property :frame_time, :float, true
  property :frame_range, :float_pair, true
  property :paused, :bool, true

  def initialize(data)
    super data

    @sprite = self.owner.sprite_component
    @paused = data.fetch("paused", "false") == "true"
    @frame_time = data.fetch("frame_time", 1 / 24).to_f
    @frame_range = data.fetch("frame_range", [0, 0xffff]).map{|f| f.to_i }
    @time = 0

    self.register_event :update, :animate
  end

  MAX_FRAME_SKIP = 10

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
    skipped = -1
    while @time >= @frame_time
      @time -= @frame_time
      next_frame

      skipped += 1
      break if skipped >= MAX_FRAME_SKIP
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

  register_component
end
