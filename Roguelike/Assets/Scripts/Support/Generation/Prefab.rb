#########################
# Prefab.rb
# Connor Hilarides
# Created 2015/02/24
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#########################

class PrefabDoorContext
  def initialize(closed)
    @closed = closed
  end

  def closed?
    @closed
  end
end

