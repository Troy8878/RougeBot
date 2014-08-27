#######################
# SpinnyThingComponent.rb
# Connor Hilarides
# Created 2014/08/25

class SpinnyThingComponent < ComponentBase
  def initialize(data)
    super data

    self.transform = self.owner.get_component "TransformComponent"
    self.register_event :update, :on_update

    self.speed = data[:speed].to_f || 1.0
  end

  def on_update(e)
    self.transform.rotation.y += e.data.dt * self.speed
  end

  register_component 'SpinnyThingComponent'
end
