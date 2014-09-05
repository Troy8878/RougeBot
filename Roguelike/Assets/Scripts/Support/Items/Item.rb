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
  attr_reader :name, :rarity

  def initialize(data)
    #Either copy an existing item or create one from JSON
    if data.is_a? Item
      @name = data.name
      @rarity = data.rarity
    else
      @name = data.fetch("name", "Default")
      @rarity = data.fetch("rarity", 0).to_f
    end
  end

  def clone
    self.class.new self
  end

end
