#######################
# StatusMessageComponent.rb
# Connor Hilarides
# Created 2014/09/01
#######################

class StatusMessageComponent < ComponentBase
  def initialize(data)
    super data

    @time = 0
    @position = self.owner.transform_component.position
    @delete_owner = data["delete_owner"]

    register_event :update, :on_update
  end

  def on_update(e)
    @time += e.dt
    @position.y += e.dt

    if @time > 1
      if @delete_owner
        self.owner.parent.zombify!
      else
        self.owner.zombify!
      end
    end
  end

  register_component "StatusMessageComponent"
end
