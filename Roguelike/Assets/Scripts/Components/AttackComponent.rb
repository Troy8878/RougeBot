#######################
# AttackComponent.rb
# Jake Robsahm
# Created 2014/09/05
#######################

#########################Class Details##########################
# Component that allows an object to attack another object.
#######################Property Details#########################
# attack - the chance of the object hitting (?)
# damage - the range of damage an object can do
################################################################
class AttackComponent < ComponentBase
  attr_reader :attack, :damage

  def initialize(data)
    super data

    @attack = 0
    @damage = [0,0]
  end

  def equip_weapon()
    # Pull the weapon from the inventory.
    item = self.owner.inventory_component.inventory.equipment[:weapon]
    # Set the attack and damage values to the newly equipped weapon's values.
    attack = item.attack
    damage = item.damage
  end

  def attack()
  end

  register_component "AttackComponent"
end
