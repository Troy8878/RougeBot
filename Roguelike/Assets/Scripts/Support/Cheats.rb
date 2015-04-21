#########################
# Cheats.rb
# Connor Hilarides
# Created 2014/10/30
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
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

  def its_over_9000
    find_entity("Player").inventory_component.give_random_weapon 9001
  end

  def skip_level
    pos = find_entity("Player").position_component.position
    pos.x = STAIR_POSITION.x
    pos.y = STAIR_POSITION.y
  end

  def victory
    Game.switch_level 'Victory'
  end

  def seemap
    find_entity("Minimap").map_component.remove_fow
    find_entity("Minimap").raise_event :map_update, nil
  end

  def ded
    find_entity("Player").defense_component.be_attacked 9000
  end

  def help
    puts "       Cheat help       "
    puts "========================"
    puts "> cheat.skip_level      "
    puts "> cheat.its_over_9000   "
    puts "> cheat.victory         "
    puts "> cheat.god = :on       "
    puts "> cheat.god = :off      "
    puts "> cheat.seemap          "
    puts "========================"
  end
end

CHEAT_INST = CheatState.new
def cheat
  CHEAT_INST
end
