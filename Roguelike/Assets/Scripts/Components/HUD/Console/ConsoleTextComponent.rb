###########################
# ConsoleTextComponent.rb
# Connor Hilarides
# Created 2014/10/24
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
###########################

class ConsoleTextComponent < ComponentBase
  dependency "TransformComponent"

  SHIFT_DY = 0.0375
  MAX_LINES = 25

  serialized_input do |p|
    p.dependency "TransformComponent"
  end

  def initialize(data)
    super data

    @line = 0
    @pos = owner.transform_component.position

    register_event :shift_up, :shift
  end

  def shift(e)
    @pos.y += SHIFT_DY

    @line += 1
    if @line > MAX_LINES
      owner.action_group.once do
        owner.zombify!
      end
    end
  end

  register_component "ConsoleTextComponent"
end
