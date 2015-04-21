#######################
# MenuQuitBlocker.rb
# YOURNAME
# Created 2014/DATEHERE
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#######################

class MenuQuitBlocker < ComponentBase

  serialized_input do |p|
    p.bool :splash
  end
  
  def initialize(data)
    super data

    @is_splash = !!data.fetch("splash", false)

    register_event :quit, :quit
  end

  def quit(e)
    if @is_splash
      e[:quit] = false
    else
      e[:quit] = coda! "Are you sure you want to quit?"
    end
  end

  register_component
end
