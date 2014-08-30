#######################
# SpinnyThingComponent.rb
# Connor Hilarides
# Created 2014/08/25

class SpinnyThingComponent < ComponentBase
  def initialize(data)
    super data

    @transform = self.owner.get_component "TransformComponent"
    self.register_event :update, :on_update

    @speed = data.fetch("speed"){ 1 }.to_f
  end

  def on_update(e)
    @transform.rotation.y += e.dt * @speed
  end

  register_component 'SpinnyThingComponent'
end
