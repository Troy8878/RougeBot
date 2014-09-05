#######################
# Weapon.rb
# Jake Robsahm
# Created 2014/09/05
#######################

#########################Class Details##########################
#Weapon is the base class for all weapons in the game.
#It inherits all properties from the Item class.
#######################Property Details#########################
#attack - the chances of this weapon hitting (?)
#damage - the range of damage this weapon can do
################################################################

class Weapon < Item
  attr_reader :attack, :damage


  def initialize(data)
    super data

    if data.is_a? Weapon
      @attack = data.attack
      @damage = data.damage
    else
      @attack = data.fetch("attack", 1).to_f
      @damage = data.fetch("damage", 1).to_i
    end

  end

end
