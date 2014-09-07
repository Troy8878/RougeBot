#########################
# Inventory.rb
# Jake Robsahm
# Created 2014/09/05
#########################

#########################Class Details##########################
# Component that serves as an inventory.
#######################Property Details#########################
# To-do
################################################################

class Inventory
  attr_reader  :inventory

  # Constant array containing all valid equipment slots.
  EQUIPMENT_SLOTS = [:weapon, :chest, :shield]

  def initialize

    @inventory = []
    @equipment = {}

  end

  # Function to equip an item/add it to the equipment hash map
  def equip(slot)

    # Make sure slot is a valid index
    if @inventory[slot].nil?
      raise "No item in slot #{slot}"
    end

    # Store the item in a variable for now
    item = @inventory[slot]

    # Check to make sure that the slot is valid. If not, raise an exception.
    if !EQUIPMENT_SLOTS.include? item.equip_slot
      raise "Invalid equipment slot : #{item.equip_slot}"
    end

    # Remove the item from the inventory array
    @inventory[slot] = nil

    # Check if something is already equipped
    if !@equipment[item.equip_slot].nil?
      equipped = @equipment[item.equip_slot]
      @equipment[item.equip_slot] = nil
      @inventory[slot] = equipped
    end

    # Insert the item into the equipment at the appropriate slot
    @equipment[item.equip_slot] = item

  end
end
