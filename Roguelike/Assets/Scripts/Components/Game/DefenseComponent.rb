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

    total_dmg = damage - @armor
    @health -= (damage - @armor)

    if @health <= 0
      notify_death
      self.owner.zombify!
      return :kill
    end

    message = StatusMessage.new("#{total_dmg}", 1, "Red")
    message.display self.owner

    return :hit
  end

  def notify_death
    transient = self.owner.parent.create_child(
      components: {
        "TransformComponent" => self.owner.transform_component.dup_for_hash
      }
    )

    message = StatusMessage.new("👻", 2, "Black")
    message.delete_owner!
    message.display transient

    Event.raise_event :actor_death, self.owner
  end

  def heal(amount)
    @health += amount

    message = StatusMessage.new("#{amount}", 1, "Green")
    message.display self.owner
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
