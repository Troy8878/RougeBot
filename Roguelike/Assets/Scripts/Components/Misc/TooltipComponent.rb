#######################
# SpinnyThingComponent.rb
# Connor Hilarides
# Created 2014/08/25
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#######################

class TooltipComponent < ComponentBase
  serialized_input do |p|
    p.string :text, default: "Enter some text, why doncha?"
  end

  def initialize(data)
    super data

    build(data["text"])
  end

  def build(text)
    puts text
    child = owner.create_child archetype: "Items/Tooltip"
    child.raise_event :send, [:set_text_at, [0, text]]
    child.local_find("TooltipWrapper").transform_component.position.y = 0.5
  end

  register_component
end
