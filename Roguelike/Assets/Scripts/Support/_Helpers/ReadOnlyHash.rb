#########################
# ReadOnlyHash.rb
# Connor Hilarides
# Created 2014/09/07
#########################

class ReadOnlyHash
  def initialize(hash)
    @hash = hash
  end

  def [](sym)
    @hash[sym]
  end
end
