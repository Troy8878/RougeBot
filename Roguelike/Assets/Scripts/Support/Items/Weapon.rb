#######################
# Weapon.rb
# Jake Robsahm, Leonardo Saikali
# Created 2014/09/05
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
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

class Weapon < Item
  attr_reader :damage, :durability, :weaponType, :value
  alias weapon_type weaponType

  attr_reader :attributes

  DAGGER_TYPE = 0
  SABER_TYPE = 1
  AXE_TYPE = 2
  SPEAR_TYPE = 3

  LEFT = 4
  RIGHT = 5
  TOP = 6
  BOTTOM = 7

  ANIM_NAMES = {
    DAGGER_TYPE => "Dagger",
    SABER_TYPE => "Saber",
    AXE_TYPE => "Dagger",
    SPEAR_TYPE => "Spear"
  }

  ATTACK_SQUARES = {
    DAGGER_TYPE => {
      left: [ [-1, 0] ],
      right: [ [1, 0] ],
      up: [ [0, 1] ],
      down: [ [0, -1] ]
    },

    SABER_TYPE => {
      left: [ [-1, -1], [-1, 0], [-1, 1] ],
      right: [ [1, -1], [1, 0], [1, 1] ],
      up: [ [-1, 1], [0, 1], [1, 1] ],
      down: [ [-1, -1], [0, -1], [1, -1] ]
    },

    AXE_TYPE => {
      left: [ [0, -1], [-1, -1], [-1, 0], [-1, 1], [0, 1] ],
      right: [ [0, -1], [1, -1], [1, 0], [1, 1], [0, 1] ],
      up: [ [-1, 0], [-1, -1], [-1, 1], [0, 1], [1, 1], [1, 0] ],
      down: [ [-1, 0], [-1, -1], [0, -1], [1, -1], [1, 0] ]
    },

    SPEAR_TYPE => {
      left: [ [-1, 0], [-2, 0] ],
      right: [ [1, 0], [2, 0] ],
      up: [ [0, 1], [0, 2] ],
      down: [ [0, -1], [0, -2] ]
    }
  }

  def initialize(data)
    super data

    if data.is_a? Weapon
      @damage = data.damage
      @durability = data.durability
      @value = data.value
      @damage_type = data.damage_type
      @attributes = data.attributes
    else
      range = data.fetch("damage", [1, 1])
      if range.is_a?(String)
        range = range.split(' ')
        range = range[0..1].map{|s| s.to_i }
      end
      @damage = range
      @durability = data.fetch("durability", 1).to_f
      @damage_type = data.fetch("damage_type", :physical)
      @weaponType = data.fetch("weaponType", DAGGER_TYPE)
      @value = data.fetch("damage_type", 1)
      @attributes = data.fetch("attributes", [])

      @equip_slot = :weapon
    end
  end

  def equip_slot
    return :weapon
  end

  def weapon_type_name
    ANIM_NAMES[weapon_type]
  end

  def view
    puts "herpderp"
    unless @view.is_a? View
      @view = View.new(self)
    end
    @view
  end
end
