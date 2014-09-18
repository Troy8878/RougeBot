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

      register_event :mouse_move, :mouse_move
    end

    def mouse_move(e)
      position = @camera.screen_to_world(e.position)

      position.x = Math.round(position.x)
      position.z = Math.round(position.z)

      @transform.position = position
    end

    register_component "HUD::TileCursorComponent"
  end
end
