##############################
# PlayerControllerComponent.rb
# Jake Robsahm
# Created 2014/09/05
##############################

#########################Class Details##########################
# Component makes the object the player.
#######################Property Details#########################
# health - the health of the player. May be changed if we use
#the same health system for enemies.
# speed - how fast the player moves or attacks. TBD.
# inventory - an inventory object containing all of the player's
#items.
################################################################

PLAYER_INVENTORY = Inventory.new

class PlayerControllerComponent < ComponentBase

  # Initialize the properties of the PlayerController
  def initialize(data)
    super data

    # Base player stats. These might be moved to other
    # components later.
    @health = data.fetch("health", 10).to_i
    @speed = data.fetch("speed", 1).to_f

  end

  def on_key(e)
  end

  def on_update(e)
  end

  register_component "PlayerControllerComponent"
end
