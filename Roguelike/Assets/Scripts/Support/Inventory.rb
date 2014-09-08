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

  INVENTORY_CAPACITY = 30

  def initialize

    @inventory = []
    @equipment = {}

  end

  # Function to check if there is room in the inventory
  def room_in_inventory
    (1..INVENTORY_CAPACITY).each do |i|
      return true if @inventory[i - 1].nil?
    end

    return false
  end

  # Function to add items to the inventory.
  def pickup(item)
    # Scan over the array for the first open slot
    insert_index = -1
    @inventory.each_with_index do |item, index|
      if item.nil?
        insert_index = index
        break
      end
    end

    # If there was no open slot, check if we have room left
    if insert_index == -1 && @inventory.count < INVENTORY_CAPACITY
      insert_index = @inventory.count
      @inventory << nil
    end

    # No room in the inventory
    if insert_index == -1
      raise "Inventory full"
    end

    # Insert the item into the inventory.
    @inventory[insert_index] = item
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
      # Transfer the equipped item to the inventory.
      equipped = @equipment[item.equip_slot]
      @equipment[item.equip_slot] = nil
      @inventory[slot] = equipped
    end

    # Insert the item into the equipment at the appropriate slot
    @equipment[item.equip_slot] = item
  end
end
