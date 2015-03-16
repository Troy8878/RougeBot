#######################
# StatusMessage.rb
# Leonardo Saikali
# Created 2014/10/20
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class StatusMessage
  def initialize(message, scale, color, fsize = 60, speed = 1)
    @message = message
    @scale = scale
    @color = color
    @fsize = fsize
    @speed = speed
    @delete_owner = false
  end

  def display(target)
    scale = Vector.new(2, 0.5, 1, 1) * @scale

    target.create_child(
      archetype: "Flare/StatusMessage",
      components: {
        "TransformComponent" => {
          "scale" => [scale.x, scale.y, scale.z, 1]
        },
        "TextComponent" => {
          "texts" => [ @message ],
          "text_color" => @color,
          "font_size" => @fsize
        },
        "StatusMessageComponent" => {
          "delete_owner" => @delete_owner,
          "speed" => @speed
        }
      }
    )
  end

  def delete_owner!
    @delete_owner = true
  end
end
