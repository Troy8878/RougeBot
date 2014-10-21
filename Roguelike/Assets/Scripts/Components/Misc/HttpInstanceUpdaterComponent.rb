#########################
# HttpInstanceUpdaterComponent.rb
# Connor Hilarides
# Created 2014/09/01
#########################

class HttpInstanceUpdaterComponent < ComponentBase
  def initialize(data)
    super data

    register_event :update, :on_update
  end

  def on_update(e)
    Http::INSTANCE.update
  end

  register_component "HttpInstanceUpdaterComponent"
end
