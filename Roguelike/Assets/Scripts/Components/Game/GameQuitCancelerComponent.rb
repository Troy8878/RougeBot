#########################
# GameQuitCancelerComponent.rb
# Connor Hilarides
# Created 2014/09/01
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

class GameQuitCancelerComponent < ComponentBase

  serialized_input do |p|
  end
  
  def initialize(data)
    super data

    register_event :quit, :on_quit
  end

  def on_quit(e)
    Event.handled = true

    owner.children.each do |entity|
      entity.raise_event :quit, e
    end

    return if e[:quit] == false

    e[:quit] = false
    owner.raise_event :pause, nil
  end

  register_component "GameQuitCancelerComponent"
end
