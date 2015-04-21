#########################
# Texture.rb
# Connor Hilarides
# Created 2014/10/18
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#########################

class Texture
  def inspect
    "#<Texture:0x#{object_id.to_s(16)} name=#{name}, size=#{width}x#{height}>"
  end

  def size
    [width, height]
  end
end

module Components

  class TextureComponent < ComponentBase
    include NativeEnumerable
  end
  
end
