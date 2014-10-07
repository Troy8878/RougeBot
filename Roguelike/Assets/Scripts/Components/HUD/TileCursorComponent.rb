#######################
# TileCursor.rb
# Connor Hilarides
# Created 2014/09/17
#######################

module HUD
  class TileCursorComponent < ComponentBase
    def initialize(data)
      super data

      @camera = data["camera"]
      @transform = self.owner.transform_component

      register_event :update, :first_update
    end

    def first_update(e)
      remove_event :update

      @camera = find_entity("CameraRoot").find_entity(@camera).camera_component
      @mouse_pos = Vector.new

      register_event :mouse_move, :mouse_move
    end

    def mouse_move(e)
      @mouse_pos = e.position

      position = @camera.screen_to_world(@mouse_pos)

      position.x = Math.round(position.x)
      position.z = Math.round(position.z)

      @transform.position = position
    end

    register_component "HUD::TileCursorComponent"
  end
end
