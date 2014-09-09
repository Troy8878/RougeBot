#######################
# SpinnyThingComponent.rb
# Connor Hilarides
# Created 2014/08/25
#######################

class SpinnyThingComponent < ComponentBase
  attr_accessor :speed

  def initialize(data)
    super data

    @speed = data.fetch("speed", 1).to_f
    @transform = self.owner.transform_component

    self.register_event :update, :on_update
  end

  def on_update(e)
    @transform.rotation.z += e.dt * @speed
  end

  register_component "SpinnyThingComponent"
end
