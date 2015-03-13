#######################
# ItemRandomization.rb
# Connor Hilarides
# Created 2015/02/04
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class ItemRandomizer
  def initialize(category_order)
    @order = category_order
    @categories = {}
  end

  def add(trait)
    cat = trait.category
    @categories[cat] ||= []
    @categories[cat] << trait
  end

  def randomize_base(base_item)
    @order.each do |category|
      @categories[category].rand_item.apply base_item
    end
  end
end

class RandomTrait
  attr_reader :category, :weight, :target_level

  def initialize(category, weight, target_level)
    @category = category
    @weight = weight
  end

  def apply(item)
    raise "RandomTrait#apply is unimplemented in the base class"
  end
end

class WeaponClassTrait < RandomTrait
  attr_reader :weapon_class

  def initialize(options)
    super :weapon_class, options[:weight], options[:target_level]
    @weapon_class = options[:weapon_class]
  end

  def apply(item)

  end
end

class MaterialTrait < RandomTrait
  attr_reader :material, :damage_mod

  def initialize(options)
    super :material, options[:weight], options[:target_level]

    @material = options[:material]
    @damage_mod = options[:damage_mod]
  end

  def apply(item)
    item.damage[0] += damage_mod[0]
    item.damage[1] += damage_mod[1]
  end
end

########################################################################
#                  Stuff for the random generator                      #
########################################################################

WEAPON_GENERATOR = ItemRandomizer.new [:weapon_class, :material]


# Weapon Classes
WEAPON_GENERATOR.add WeaponClassTrait.new(
  weapon_class: "Dagger",
  weight: 1,
)

# Weapon Materials
WEAPON_GENERATOR.add MaterialTrait.new(
  material: "Iron",
  weight: 5, target_level: 3,
  damage_mod: [-1, 5]
)
