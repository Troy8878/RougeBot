#######################
# PlayerHealthComponent.rb
# Connor Hilarides
# Created 2014/09/05
#######################

class PlayerHealthComponent < ComponentBase
  def initialize(data)
    super data

    register_event :health_changed, :on_change
  end

  def on_change(e)
    health = e[:new_value]

    text = self.owner.text_component
    text.texts = ["Health: #{health.to_i}"]
  end

  register_component "PlayerHealthComponent"
end
