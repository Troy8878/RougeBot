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

    if self.owner.player_controller_component
      @inventory = PLAYER_INVENTORY
    else
      @inventory = Inventory.new
    end
    
    @inventory.on_change { self.do_update }
  end

  def do_update

  end

  def finalize
    @inventory.clear_callback
  end
end
