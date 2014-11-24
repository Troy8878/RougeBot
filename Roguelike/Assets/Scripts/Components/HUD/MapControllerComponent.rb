###########################
# MapControllerComponent.rb
# Jake Robsahm
# Created 2014/09/24
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
###########################

#########################Class Details##########################
# Component that controls the size and location of the minimap.
#######################Property Details#########################
# transform - the transform component of the minimap.
# original_pos - the original position of the minimap.
# maximized - boolean to store if the map is maximized or not.
################################################################
class MapControllerComponent < ComponentBase

  def initialize(data)
    super data

    @transform = self.owner.transform_component
    @original_pos = @transform.position.dup
    @maximized = false
    
    # Look out for key presses.
    self.register_event :minimap_toggle, :on_toggle
  end

  def on_toggle(e)
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
    # We move the map to the center, and scale it.
    @transform.position = Vector.new(0, 0, -0.1)
    @transform.scale = Vector.scalar(2)
    # We need to record that the map is now maximized.
    @maximized = true
  end

  def minimize
    # Move everything back to the original location and scale.
    @transform.position = @original_pos
    @transform.scale = Vector.scalar(1)
    # Record that the map is no longer maximized.
    @maximized = false
  end

  register_component "MapControllerComponent"
end
