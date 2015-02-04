#######################
# AITest.rb
# Connor Hilarides
# Created 2015/02/03
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class AITestComponent < ComponentBase

  dependency "AiComponent"

  # Describe what goes in the json file
  serialized_input do |p|
  end

  def initialize(data)
    super data

    register_event :ai_complete, :ai_complete

    self.owner.ai_component.enqueue "AIDerp", owner, owner
  end

  def ai_complete(e)
    puts "AI Result: #{e}"
  end

  register_component
end
