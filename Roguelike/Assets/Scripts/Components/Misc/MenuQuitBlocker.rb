#######################
# MenuQuitBlocker.rb
# YOURNAME
# Created 2014/DATEHERE
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class MenuQuitBlocker < ComponentBase

  serialized_input do |p|
  end
  
  def initialize(data)
    super data

    register_event :quit, :quit
  end

  def quit(e)
    e[:quit] = coda! "Are you sure you want to quit?"
  end

  register_component
end
