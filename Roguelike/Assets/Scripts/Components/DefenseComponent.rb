######################
# DefenseComponent.rb
# Jake Robsahm
# Created 2014/09/05

class DefenseComponent < ComponentBase
  def initialize(data)
    super data

    @equipment = {}

  end

  def equip_item(slot, item)
    @equipment[slot] = item
  end

  def defense
    #todo later
  end

  def armor
    #todo later
  end



end
