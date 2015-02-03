#######################
# HealthBarComponent.rb
# Enrique Rodriguez
# Created 2014/12/05
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class HealthBarComponent < ComponentBase

  serialized_input do |p|
  end
  
  def initialize(data)
    super data
    
    register_event :health_changed, :on_change
    register_event :update, :first_update
  end
  
  def first_update(e)
    remove_event :update
    @max_health = owner.parent.defense_component.health
  end

  def on_change(e)
    health = owner.parent.defense_component.health

    bar = self.owner.transform_component
    bar.scale = Vector.new *[health / @max_health, 0.5, 1]
  end

  register_component "HealthBarComponent"
end