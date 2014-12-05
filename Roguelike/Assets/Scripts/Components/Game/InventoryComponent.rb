#######################
# InventoryComponent.rb
# Jake Robsahm
# Created 2014/09/11
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

#########################Class Details############################
# A component that contains the inventory of the entity.
#######################Property Details###########################
# inventory - the inventory contained by the component.
##################################################################

class InventoryComponent < ComponentBase
  attr_reader :inventory

  def initialize(data)
    super data

    if data["is_player"] == true
      @inventory = PLAYER_INVENTORY

      # I need to delay it because otherwise the hotbar won't render it
      seq = owner.action_sequence :give_random_weapon
      seq.delay 0
      seq.once do
        give_random_weapon GAME_STATE[:floor]
      end
    else
      @inventory = Inventory.new
    end
    
    @inventory.on_change {|*a| self.do_update(*a) }
  end

  def give_random_weapon(level)
    @inventory.pickup ItemGenerate.generate_weapon({}, level)
  end

  def do_update(*args)
    owner.local_event :inventory_update, args
  end

  def finalize
    @inventory.clear_callback
  end

  register_component "InventoryComponent"
end
