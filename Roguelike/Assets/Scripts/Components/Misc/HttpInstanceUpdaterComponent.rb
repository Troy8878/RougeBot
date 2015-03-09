#########################
# HttpInstanceUpdaterComponent.rb
# Connor Hilarides
# Created 2014/09/01
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

class HttpInstanceUpdaterComponent < ComponentBase

  serialized_input do |p|
  end
  
  def initialize(data)
    super data

    register_event :update, :on_update
  end

  def on_update(e)
    Http::INSTANCE.update
  end

  register_component "HttpInstanceUpdaterComponent"
end
