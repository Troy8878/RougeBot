#######################
# Projectile.rb
# Troy
# Created 2014/12/02
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#######################

# Don't load this before its base class Item
class Item
end

class Projectile < Item
  attr_reader :damage, :durability, :speed, :value, :element1, :element2

  NONE = 0
  PHYSICAL_ELEM = 1
  FIRE_ELEM = 2
  ICE_ELEM = 3
  ELECTRIC_ELEM = 4
  RADIOACTIVE_ELEM = 5
  EXPLOSIVE_ELEM = 6
  
  CRUSHING_ELEM = 7
  PLASMIC_ELEM = 8
  HYPERBOREAL_ELEM = 9
  ZAPPING_ELEM = 10
  NUCLEAR_ELEM = 11
  HIGHEXPLOSIVE_ELEM = 12

  # We'll work with elements once it's actually time to implement them!

  def initialize(data)
    super data
    
    # We SHOULD get a Projectile...
    if data.is_a? Projectile
      @damage = data.damage
      @durability = data.durability
      @value = data.value
      @speed = data.speed
      
      # Elements: If 1 & 2 are equal it becomes a better version of both
      if(data.element1 == data.element2)
        # At this point we need to check the specific element
        if(data.element1 == PHYSICAL_ELEM)
          data.element1 = CRUSHING_ELEM
          data.element2 = NONE
        elsif(data.element1 == FIRE_ELEM)
          data.element1 = PLASMIC_ELEM
          data.element2 = NONE
        elsif(data.element1 == ICE_ELEM)
          data.element1 = HYPERBOREAL_ELEM
          data.element2 = NONE
        elsif(data.element1 == RADIOACTIVE_ELEM)
          data.element1 = NUCLEAR_ELEM
          data.element2 = NONE
        elsif(data.element1 == ELECTRIC_ELEM)
          data.element1 = ZAPPING_ELEM
          data.element2 = NONE
        elsif(data.element1 == EXPLOSIVE_ELEM)
          data.element1 = HIGHEXPLOSIVE_ELEM
          data.element2 = NONE
        end
      
      # No match means no super-element.  Two elements are still good though!
      else
        @element1 = data.element1
        @element2 = data.element2
      end
      
    # ...but if we don't we'll be forced to default to something else
    else
      range = data.fetch("damage", [1, 1])
      if range.is_a?(String)
        range = range.split(' ')
        range = range[0..1].map{|s| s.to_i }
      end
      @dmamage = range
      @durability = data.fetch("durability", 1).to_f
      @element1 = data.fetch("element1", PHYSICAL_ELEM)
      @element2 = data.fetch("element2", NONE)
      @value = data.fetch("value", 1)
      @speed = data.fetch("speed", 0)

      @equip_slot = :projectile
    end
  end

  def equip_slot
    return :projectile
  end

end