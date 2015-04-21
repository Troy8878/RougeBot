#######################
# TileCursor.rb
# Connor Hilarides
# Created 2014/09/17
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#######################

module HUD
  class TileCursorComponent < ComponentBase
    dependency "TransformComponent"

    serialized_input do |p|
      p.dependency "TransformComponent"
      p.string :camera, required: true, semantics: :entity_name
    end

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
      @player = find_entity("Player")
      @mouse_pos = Vector.new

      register_event :mouse_move, :mouse_move
      register_event :update, :on_update
    end

    def update_transform
      position = @camera.screen_to_world(@mouse_pos)
      playerpos = @player.position_component.position

      diff = position - playerpos

      if diff.length2 > 9
        @transform.position = Vector.new(0, 0, 500)
        return
      end

      angle = Math.atan2(diff.y, diff.x)
      angle -= Math::PI / 4
      angle %= 2 * Math::PI
      angle = (angle / (2 * Math::PI) * 4).to_i

      case angle
      when 0
        position = playerpos + Vector.new(0, 1)
      when 1
        position = playerpos + Vector.new(-1, 0)
      when 2
        position = playerpos + Vector.new(0, -1)
      when 3
        position = playerpos + Vector.new(1, 0)
      end

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
