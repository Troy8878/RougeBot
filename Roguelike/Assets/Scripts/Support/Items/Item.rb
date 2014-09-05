#######################
# Item.rb
# Jake Robsahm
# Created 2014/09/05

class Item
  def initialize(data)
    #Fetch item name from the JSON data
    @name = data.fetch("name", "Default")
    #Chances of the random map generator creating this item
    @rarity = data.fetch("rarity", 0).to_f

  end

  def clone
    self.class.new self
  end

end
