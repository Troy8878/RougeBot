#######################
# StatusMessage.rb
# Leonardo Saikali
# Created 2014/10/20
#######################

class StatusMessage
  def initialize(message, scale, color)
    @message = message
    @scale = scale
    @color = color
  end

  def display(target)
    scale = Vector.new(1, 0.5, 1, 1) * @scale

    target.create_child(
      archetype: "Flare/StatusMessage",
      components: {
        "TransformComponent" => {
          "scale" => [scale.x, scale.y, scale.z, 1]
        },
        "TextComponent" => {
          "texts" => [ @message ],
          "text_color" => @color
        }
      }
    )
  end
end
