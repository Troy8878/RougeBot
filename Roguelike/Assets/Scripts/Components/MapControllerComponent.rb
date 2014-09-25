###########################
# MapControllerComponent.rb
# Jake Robsahm
# Created 2014/09/24
###########################

class MapControllerComponent < ComponentBase

  def initialize(data)
    super data

    @transform = self.owner.transform_component
    @original_pos = @transform.position.dup
    @maximized = false
    
    self.register_event :key_down, :on_key
  end

  def on_key(e)
    return unless e.plain_char == 'M'

    if maximized?
      minimize
    else
      maximize
    end
  end

  def maximized?
    return @maximized
  end

  def maximize
    @transform.position = Vector.zero
    @transform.scale = Vector.scalar(2)
    @maximized = true
  end

  def minimize
    @transform.position = @original_pos
    @transform.scale = Vector.scalar(1)
    @maximized = false
  end

  register_component "MapControllerComponent"
end
