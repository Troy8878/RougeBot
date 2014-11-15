#######################
# Item.rb
# Jake Robsahm
# Created 2014/09/05
#######################

#########################Class Details###########################
# Item is the base class of all items in the game.
#######################Property Details##########################
# name - the name of the item
# rarity - chances of the random map generator creating this item
#################################################################

class Item
  attr_reader :name, :value, :equip_slot
  attr_accessor :item_level

  # Data to determine what the item looks like
  attr_reader :view

  def initialize(data)
    #Either copy an existing item or create one from JSON
    if data.is_a? Item
      @name = data.name.dup
      @value = data.value.dup
      @view = data.view.dup
    else
      @name = data.fetch("name", "Default")
      @value = data.fetch("value", 0).to_f
      @view = ItemView.new(nil, self)
    end
    # Makes sure you aren't equipping a base item you naughty girl
    @equip_slot = :invalid
  end

  def clone
    self.class.new self
  end

  # Function to check if an item is equippable.
  def equippable?
    return Inventory::EQUIPMENT_SLOTS.include? self.equip_slot
  end


end
