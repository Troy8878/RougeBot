#######################
# TileCursor.rb
# Connor Hilarides
# Created 2014/09/17
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

module HUD
  class TileCursorComponent < ComponentBase
    def initialize(data)
      super data

      @camera = data["camera"]
      @transform = self.owner.transform_component

      register_event :update, :first_update
      register_event :on_pause, :on_pause
    end

    def on_pause(val)
      if val
        owner.raise_event :send, [:tint=, [Vector.zero]]
      else
        owner.raise_event :send, [:tint=, [Vector.one]]
      end
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
