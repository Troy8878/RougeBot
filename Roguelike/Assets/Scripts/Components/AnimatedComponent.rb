#######################
# AnimatedComponent.rb
# Connor Hilarides
# Created 2014/09/01

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
    @paused = data.fetch("paused", "false") =~ /true/i
    @frame_time = data.fetch("frame_time", 1 / 24).to_f
    @time = 0

    save_frame_range data

    self.register_event :update, :animate
  end

  # fetch out the frame_range field
  def save_frame_range(data)
    range = data.fetch("frame_range", [0, 0xffff])
    if range.is_a?(String)
      range = range.split(' ')
      range = range[0..1].map{|s| s.to_i }
    end

    @frame_range = range
  end

  def animate(e)
    return if @paused

    @time += e.dt

    while @time >= @frame_time
      @time -= @frame_time
      next_frame
    end
  end

  def next_frame
    i = @sprite.texture_index + 1
    if i >= @sprite.texture_count || i > @frame_range[1]
      i = @frame_range[0]
    end

    @sprite.texture_index = i
  end

  register_component "AnimatedComponent"
end
