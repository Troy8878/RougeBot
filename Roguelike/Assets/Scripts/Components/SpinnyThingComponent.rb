#######################
# SpinnyThingComponent.rb
# Connor Hilarides
# Created 2014/08/25
#######################

class SpinnyThingComponent < ComponentBase
  attr_accessor :speed
  attr_reader :axis

  def initialize(data)
    super data

    @speed = data.fetch("speed", 1).to_f
    @transform = self.owner.transform_component
    self.axis = data.fetch("change_method", "z")

    self.register_event :update, :on_update
  end

  def on_update(e)
    @transform.rotation.send(
      @set_axis,
      @transform.rotation.send(@get_axis) + e.dt * speed
    )
  end

  def axis=(a)
    @axis = a
    @get_axis = @axis.to_sym
    @set_axis = "#{@axis}=".to_sym
  end

  register_component "SpinnyThingComponent"
end
