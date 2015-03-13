#######################
# DefenseComponent.rb
# Claire Robsahm, Leonardo Saikali
# Created 2014/09/05
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

#########################Class Details##########################
# Component that allows an object to defend against attack.
#######################Property Details#########################
# Equipment - an array of all equipment the object has. This
# determins the ultimate values of armor and defense.
################################################################

class DefenseComponent < ComponentBase
  include Actor

  attr_accessor :health, :armor

  # Property viewer declarations
  property :health, :int, true
  property :armor, :float, true

  serialized_input do |p|
    p.float :max_health, default: 25.0
    p.float :health, default: 10.0
  end

  def initialize(data)
    super data

    @armor = 0
    @health = data.fetch("health", 10).to_i
  end

  def on_access
    self.owner.raise_event :health_changed, {
      kind: :touch,
      amount: 0,
      new_value: @health
    }
  end

  def be_attacked(damage)
    total_dmg = damage / Math.log(@armor + Math::E)
    total_dmg = total_dmg.to_i
    total_dmg = 1 if total_dmg < 1
    total_dmg = 0 if owner.player_controller_component && cheat.god?

    @health -= total_dmg

    # Notify that the health has changed
    self.owner.raise_event :health_changed, { 
      kind: :damage,
      amount: -total_dmg,
      new_value: @health
    }

    if @health <= 0
      notify_death
      self.owner.zombify!
      return :kill
    end

    default_tint = Vector.new(1, 1, 1, 1)
    high_tint = Vector.new(5, 0.5, 0.5, 1)

    sprite = self.owner.sprite_component || self.owner.local_find("Sprite").sprite_component
    tint = sprite.tint
    seq = self.owner.action_sequence :hit_flash
    seq.interpolate tint, from: default_tint, to: high_tint, over: (0.1).seconds
    seq.interpolate tint, from: high_tint, to: default_tint, over: (0.1).seconds

    message = StatusMessage.new("#{total_dmg}", 1, "Red, 1")
    message.display self.owner

    return :hit
  end

  def heal(amt)
    @health += amt
    
    self.owner.raise_event :health_changed, {
      kind: :heal,
      amount: amt,
      new_value: @health
    }

    message = StatusMessage.new("#{amt}", 1, "Green, 0.8")
    message.display self.owner
  end

  def notify_death
    return unless self.owner.parent

    drop_random_weapon

    transient = self.owner.parent.create_child components: {
      "TransformComponent" => self.owner.transform_component.dup_for_hash
    }

    message = StatusMessage.new("👻", 2, "Cyan")
    message.delete_owner!
    message.display transient

    owner.raise_event :actor_death, self.owner
  end

  def drop_random_weapon
    return unless Random.die_roll(6) > 4

    tile = current_tile
    weap = ItemGenerate.generate_weapon({}, GAME_STATE[:floor])
    tile.drop_item weap
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
    @armor = 0

    # If the chest is equipped, add it to the total defense and armor.
    if !chest.nil?
      @armor += chest.armor
    end

    # If the shield is equipped, add it to the total defense and armor.
    if !shield.nil?
      @armor += shield.armor
    end
  end

  def armor
    #todo later
  end

  register_component "DefenseComponent"
end
