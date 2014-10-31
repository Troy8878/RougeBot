#######################
# InventoryComponent.rb
# Jake Robsahm
# Created 2014/09/11
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

      level = 1

      seq = owner.action_sequence :add_items
      seq.delay 1.5
      8.times do
        seq.delay 0.5
        seq.once { give_random_weapon 5 }
        level += 2
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
