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
      @inventory.initialize

      seq = owner.action_sequence :add_items
      seq.delay 0
      5.times do
        seq.delay 0.1
        seq.once { give_random_weapon 1 }
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
