#######################
# HotbarComponent.rb
# Jake Robsahm
# Created 2014/10/30
#######################

class HotbarComponent < ComponentBase
  def initialize(data)
    super data

    @target_name = data["target_name"]

    register_event :update, :first_update
  end

  def first_update(e)
    remove_event :update

    @target = find_entity(@target_name)
    @target.proxy_event :inventory_update, self, :on_update
  end

  def on_update(inv)

  end

  register_component "HotbarComponent"
end
