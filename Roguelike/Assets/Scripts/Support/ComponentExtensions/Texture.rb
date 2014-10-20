#########################
# Texture.rb
# Connor Hilarides
# Created 2014/10/18
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
    def each(&block)
      i = 0
      while i < count
        elem = self[i]
        block.call elem
        i += 1
      end
      self
    end
  end
end
