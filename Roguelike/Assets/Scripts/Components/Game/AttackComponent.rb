#######################
# AttackComponent.rb
# Jake Robsahm, Leonardo Saikali
# Created 2014/09/05
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

#########################Class Details##########################
# Component that allows an object to attack another object.
#######################Property Details#########################
# attack - the chance of the object hitting (?)
# damage - the range of damage an object can do
################################################################

SLASH = Sound.load "SFX/Slash"
UGH = Sound.load "SFX/Ugh"

class AttackComponent < ComponentBase
  include Actor

  attr_reader :damage, :weapon_level

  property :damage, :float_pair, true

  def initialize(data)
    super data

    @damage = data.fetch("damage", [2,4])
    @weapon_level = 1

    actor_sub_init
  end

  def equip_weapon
    # Pull the weapon from the inventory.
    item = self.owner.inventory_component.inventory.equipment[:weapon]
    # Set the attack and damage values to the newly equipped weapon's values.
    damage = item.damage
  end

  def swing_melee_weapon(direction)
    tile = current_tile

  end

  def do_attack(target)
    return if target == owner

    if owner.player_controller_component
      SLASH.play
    else
      UGH.play
    end

    dmg = Random.int_range_inc *@damage
    #puts "Attack for #{dmg} damage!"

    result = target.defense_component.be_attacked(dmg)

    # All our data for this attack
    event_data = {
      attacker: self.owner,
      defender: target,
      result: result,
      damage_roll: dmg
    }

    # Send out the final result
    case result
    when :hit
      Event.raise_event :attack_hit, event_data
    end
  end

  def damage=(value)
    old = @damage
    @damage = value
    self.owner.raise_event :damage_changed, {
      old: old,
      value: @damage
    }
  end

  def weapon_level=(value)
    @weapon_level = value
    self.owner.raise_event :damage_changed, {
      old: @damage,
      value: @damage
    }
  end

  register_component "AttackComponent"
end
