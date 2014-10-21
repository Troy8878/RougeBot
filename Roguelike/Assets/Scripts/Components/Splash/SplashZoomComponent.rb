#########################
# SplashZoomComponent.rb
# Connor Hilarides
# Created 2014/09/01
#########################

class SplashZoomComponent < ComponentBase
  def initialize(data)
    super data

    @transform = self.owner.transform_component

    register_event :update, :on_update
  end

  def on_update(e)
    @transform.position.z -= e.dt * 2
  end

  register_component "SplashZoomComponent"
end
