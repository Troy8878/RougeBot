##############################
# PlayerControllerComponent.rb
# Jake Robsahm
# Created 2014/09/05
##############################

class PlayerControllerComponent < ComponentBase
  # Initialize the properties of the PlayerController
  def initialize(data)
    super data

    #Base player stats. These might be moved to other
    #components later.
    @health = data.fetch("health", 10).to_i
    @speed = data.fetch("speed", 1).to_f
    @inventory = Inventory.new

  end

  def on_key(e)
  end

  def on_update(e)
  end


end

class Inventory
end
