#########################
# Cheats.rb
# Connor Hilarides
# Created 2014/10/30
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

class CheatState
  def initialize
    @data = {
      god: :off
    }
  end

  def god?
    @data[:god] == :on
  end

  def god=(mode)
    @data[:god] = mode
  end
end

CHEAT_INST = CheatState.new
def cheat
  CHEAT_INST
end
