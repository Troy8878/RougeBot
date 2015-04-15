#######################
# GoToSplash.rb
# Connor Hilarides
# Created 2014/11/14
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class GoToSplash < ComponentBase

  serialized_input do |p|
  end
  
  def initialize(data)
    super data

    # Prime the GC
    ary = []
    1000.times do
      ary << Object.new
    end

    @frames = 0

    #precache these beautiful 4K textures
    Texture.load "MainMenu/MainMenu.png"
    Texture.load "MainMenu/OtherMenu.png"

    register_event :update, :update
  end

  def update(e)
    @frames += 1
    return unless @frames > 20

    Game.switch_level "DPSplash"
  end

  register_component
end
