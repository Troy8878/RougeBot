#######################
# AttackComponent.rb
# Jake Robsahm
# Created 2014/09/05
#######################

class AttackComponent < ComponentBase
  attr_reader :attack, :damage

  def initialize(data)
    super data

    @attack = 0
    @damage = [0,0]
  end

  def equip_item(item)
    @item = item
    @attack = item.attack
    @damage = item.damage
  end

  def attack()
  end

end
