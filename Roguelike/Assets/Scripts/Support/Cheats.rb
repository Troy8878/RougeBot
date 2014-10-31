#########################
# Cheats.rb
# Connor Hilarides
# Created 2014/10/30
#########################

class CheatState
  def initialize
    @data = {}
  end

  def self.instance
    @@instance
  end

  def god?
    @data.include? :god && @data[:god] == :on
  end

  def god=(mode)
    @data[:god] = mode
  end

  @@instance = CheatState.new
end

class Cheat
  def god(mode)
    CheatState.instance.god = mode
  end
end

def enable_cheat_codes
  cheat_inst = Cheat.new
  def cheat
    cheat_inst
  end
end
