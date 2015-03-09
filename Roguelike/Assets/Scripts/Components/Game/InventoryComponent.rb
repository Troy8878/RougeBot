#######################
# InventoryComponent.rb
# Claire Robsahm
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

  serialized_input do |p|
    p.bool :is_player, required: true, default: false
  end

  def initialize(data)
    super data

    if data["is_player"] == true
      @inventory = PLAYER_INVENTORY
      give_random_weapon GAME_STATE[:floor]

      # I need to delay it because otherwise the hotbar won't render it
      refresh_hotbar
    else
      @inventory = Inventory.new
    end
    
    @inventory.on_change {|*a| self.do_update(*a) }
  end

  def give_random_weapon(level)
    if PLAYER_INVENTORY.room_in_inventory == true
      @inventory.pickup ItemGenerate.generate_weapon({}, level)
    end
  end

  def do_update(*args)
    owner.local_event :inventory_update, args
  end

  def finalize
    @inventory.clear_callback
  end

  def refresh_hotbar
    seq = owner.action_sequence :refresh
    seq.delay 0
    seq.once do
      @inventory.update_all
    end
  end

  register_component "InventoryComponent"
end
