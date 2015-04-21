#######################
# ConsoleBeGone.rb
# Connor Hilarides
# Created 2014/12/05
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#######################

class ConsoleBeGone < ComponentBase

  serialized_input do |p|
  end
  
  def initialize(data)
    super data

    register_event :update, :update
  end

  def update(e)
    AryStreamBuffer.game_console.peek.replace([])
  end

  register_component
end
