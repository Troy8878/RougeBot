#########################
# ReadOnlyHash.rb
# Connor Hilarides
# Created 2014/09/07
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#########################

class ReadOnlyHash
  def initialize(hash)
    @hash = hash
  end

  def [](sym)
    @hash[sym]
  end
end
