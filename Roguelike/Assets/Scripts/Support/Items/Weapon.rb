#######################
# Weapon.rb
# Jake Robsahm
# Created 2014/09/05
#######################

#########################Class Details##########################
# Weapon is the base class for all weapons in the game.
# It inherits all properties from the Item class.
#######################Property Details#########################
# attack - the chances of this weapon hitting (?)
# damage - the range of damage this weapon can do
################################################################

# Because this file might get loaded first
class Item
end

#An array containing every weapon ever.
WEAPONS = []

class Weapon < Item
  attr_reader :attack, :damage, :durability


  def initialize(data)
    super data

    if data.is_a? Weapon
      @attack = data.attack
      @damage = data.damage
      @durability = data.durability
    else
      @attack = data.fetch("attack", 1).to_f
      # Extract the damage. This is harder than it sounds.
      range = data.fetch("damage", [1, 1])
      if range.is_a?(String)
        range = range.split(' ')
        range = range[0..1].map{|s| s.to_i }
      end
      @damage = range
      @durability = data.fetch("durability", 1).to_f

      @equip_slot = :weapon
    end

    def equip_slot
      return :weapon
    end

  end

end
