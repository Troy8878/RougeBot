#######################
# AnimatedComponent.rb
# Connor Hilarides
# Created 2014/09/01
#######################

##
# data properties
#   paused      (Bool)  : Starts the animation component in a paused state
#   frame_time  (Float) : Number of seconds each frame is viewed for
#   frame_range (Int x2): Begin and End frame index
#
# data example
# {
#   "paused": "false", 
#   "frame_time": "0.3",
#   "frame_range": "0 5"
# }
#
class AnimatedComponent < ComponentBase
  attr_accessor :paused, :frame_time, :frame_range, :time

  def initialize(data)
    super data

    @sprite = self.owner.get_component "SpriteComponent"
    @paused = data.fetch("paused", "false") == "true"
    @frame_time = data.fetch("frame_time", 1 / 24).to_f
    @time = 0

    save_frame_range data

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

  ##
  # fetch out the frame_range field
  # from the init data
  def save_frame_range(data)
    range = data.fetch("frame_range", [0, 0xffff])
    if range.is_a?(String)
      range = range.split(' ')
      range = range[0..1].map{|s| s.to_i }
    end

    @frame_range = range
  end

  register_component "AnimatedComponent"
end
