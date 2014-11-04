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
      @camera = find_entity("CameraRoot").find_entity(@camera).camera_component
      @mouse_pos = Vector.new

      register_event :mouse_move, :mouse_move
      register_event :update, :on_update
    end

    def update_transform
      position = @camera.screen_to_world(@mouse_pos)

      position.x = Math.round(position.x)
      position.y = Math.round(position.y)

      @transform.position = position
    end

    def mouse_move(e)
      @mouse_pos = e.position
      update_transform
    end

    def on_update(e)
      update_transform
    end

    register_component "HUD::TileCursorComponent"
  end
end
