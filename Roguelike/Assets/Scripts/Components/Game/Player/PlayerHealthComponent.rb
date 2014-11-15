#######################
# PlayerHealthComponent.rb
# Connor Hilarides
# Created 2014/09/05
#######################

class PlayerHealthComponent < ComponentBase
  def initialize(data)
    super data

    register_event :health_changed, :on_change
    register_event :damage_changed, :on_change
  end

  def on_change(e)
    health = owner.parent.defense_component.health
    dmg = owner.parent.attack_component.damage
    level = owner.parent.attack_component.weapon_level

    text = self.owner.text_component
    text.texts = ["Health: #{health.to_i}\nDamage: #{dmg}\nItem Level: #{level}"]
  end

  register_component "PlayerHealthComponent"
end
