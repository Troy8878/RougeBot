#########################
# DebugHelp.rb
# Connor Hilarides
# Created 2014/11/14
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

module Debug
  
end

class PlayerExt
  def is_kill?
    find_entity("Player").nil?
  end

  def is_kill!
    player_defenses = find_entity("Player").defense_component
    player_defenses.be_attacked(player_defenses.health)
  end
end

def player
  PlayerExt.new
end

