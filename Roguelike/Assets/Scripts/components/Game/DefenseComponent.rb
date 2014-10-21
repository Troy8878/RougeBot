#######################
# DefenseComponent.rb
# Jake Robsahm, Leonardo Saikali
# Created 2014/09/05
#######################

#########################Class Details##########################
# Component that allows an object to defend against attack.
#######################Property Details#########################
# Equipment - an array of all equipment the object has. This
# determins the ultimate values of armor and defense.
################################################################

class DefenseComponent < ComponentBase
  include Actor

  attr_accessor :health

  def initialize(data)
    super data

    @defense = 0.0
    @armor = 0
    @health = data.fetch("health", 10)
  end

  def be_attacked(attack, damage)
    return :miss if attack < @defense
    @health -= (damage - @armor)

    if @health <= 0
      current_tile.actor = nil
      self.owner.zombify!
      return :kill
    end

    return :hit
  end

  def equip_armor()
    # Pull the chest and shield items.
    chest = self.owner.inventory_component.inventory.equipment[:chest]
    shield = self.owner.inventory_component.inventory.equipment[:shield]
    @defense = 0.0
    @armor = 0

    # If the chest is equipped, add it to the total defense and armor.
    if !chest.nil?
      @defense += chest.defense
      @armor += chest.armor
    end

    # If the shield is equipped, add it to the total defense and armor.
    if !shield.nil?
      @defense += shield.defense
      @armor += shield.armor
    end
  end

  def defense
    #todo later
  end

  def armor
    #todo later
  end

  register_component "DefenseComponent"
end