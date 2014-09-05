#######################
# DefenseComponent.rb
# Jake Robsahm
# Created 2014/09/05
#######################

#########################Class Details##########################
# Component that allows an object to defend against attack.
#######################Property Details#########################
# Equipment - an array of all equipment the object has. This
# determins the ultimate values of armor and defense.
################################################################

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
