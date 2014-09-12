#######################
# Armor.rb
# Jake Robsahm
# Created 2014/09/11
#######################

#########################Class Details##########################
# Armor is the base class for all armor in the game.
# It inherits all properties from the Item class.
#######################Property Details#########################
# defense - the chances of dodging an enemy attack.
# armor - the amount of damage reduction this item provides.
################################################################

# Because this file might get loaded first
class Item
end

#An array containing every armor ever.
ARMOR = []

class Weapon < Item
  attr_reader :defense, :armor, :equip_slot

  def initialize(data)
    super data

    if data.is_a? Weapon
      @defense = data.defense
      @armor = data.armor
      @equip_slot = data.equip_slot
    else
      @attack = data.fetch("defense", 1).to_f
      @armor = data.fetch("armor", 0).to_i
      # Make sure this is chest or shield.
      @equip_slot = data.fetch("equip_slot", "chest").to_sym
    end
  end

end
