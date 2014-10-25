#######################
# AttackComponent.rb
# Jake Robsahm, Leonardo Saikali
# Created 2014/09/05
#######################

#########################Class Details##########################
# Component that allows an object to attack another object.
#######################Property Details#########################
# attack - the chance of the object hitting (?)
# damage - the range of damage an object can do
################################################################
class AttackComponent < ComponentBase
  attr_accessor :attack, :damage

  def initialize(data)
    super data

    @attack = data.fetch("attack", 0)
    @damage = data.fetch("damage", [2,4])
  end

  def equip_weapon
    # Pull the weapon from the inventory.
    item = self.owner.inventory_component.inventory.equipment[:weapon]
    # Set the attack and damage values to the newly equipped weapon's values.
    attack = item.attack
    damage = item.damage
  end

  def do_attack(target)
    # We need to come up with a roll value. WHAT ARE WE ROLLING
    att = Random.die_roll 20 + @attack
    dmg = Random.int_range_inc *@damage

    result = target.defense_component.be_attacked(att, dmg)

    # All our data for this attack
    event_data = {
      attacker: self.owner,
      defender: target,
      result: result,
      attack_roll: att,
      damage_roll: dmg
    }

    # Send out the final result
    case result
    when :miss
      Event.raise_event :attack_miss, event_data
    when :hit
      Event.raise_event :attack_hit, event_data
    end
  end

  register_component "AttackComponent"
end
