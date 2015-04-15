#######################
# HotbarComponent.rb
# Connor Hilarides
# Created 2014/10/30
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class HotbarComponent < ComponentBase

  serialized_input do |p|
    p.integer :slot,        required: true
    p.string  :target_name, required: true, semantics: :entity_name
  end

  def initialize(data)
    super data

    @target_name = data["target_name"]
    @inv_slot = data["slot"].to_i

    register_event :update, :first_update
    register_event :quick_equip, :quick_equip
    register_event :click_equip, :click_equip
    register_event :drop_selected, :drop_selected
    register_event :extract_selected, :extract_selected
  end

  def first_update(e)
    remove_event :update

    if @inv_slot == 0
      select_slot
    else
      deselect_slot
    end

    @target = find_entity(@target_name)
    @target.proxy_event :inventory_update, self, :on_update
  end

  def remove_item
    holder = self.owner.local_find "ChildHolder"
    return if holder.children.empty?
    
    holder.children.last.zombify!
  end

  def display_item(item)
    return if item.nil?

    holder = self.owner.local_find "ChildHolder"
    holder.add_child item.view.create_entity
  end

  def select_slot
    @is_selected = true
    indicator = self.owner.local_find "SelectedIndicator"
    indicator.sprite_component.visible = true
  end

  def deselect_slot
    @is_selected = false
    indicator = self.owner.local_find "SelectedIndicator"
    indicator.sprite_component.visible = false
  end

  def on_update(args)
    return unless args.length == 3
    return unless args[0] == :slot
    return unless args[1] == @inv_slot

    remove_item
    display_item args[2]

    if @is_selected
      quick_equip @inv_slot
    end
  end

  def quick_equip(slot)
    deselect_slot
    return unless slot == @inv_slot
    select_slot

    atk = @target.attack_component
    inv = @target.inventory_component.inventory
    item = inv[slot]
    @item = item
    if item.nil?
      puts "Weapon Unequipped"
      atk.damage = [1,1]
      atk.weapon_level = 1
    else
      puts "Weapon `#{item.name}` with damage of #{item.damage} equipped"
      atk.damage = item.damage
      atk.weapon_level = item.item_level
    end

    PLAYER_INVENTORY.equipment[:weapon] = item
  end

  def drop_selected(tile)
    return unless @is_selected
    return unless @item

    tile.drop_item @item
    PLAYER_INVENTORY.inventory[@inv_slot] = nil
    PLAYER_INVENTORY.update_slot @inv_slot
  end

  def extract_selected(hsh)
    return unless @is_selected
    return unless @item

    hsh[:item] = @item
    PLAYER_INVENTORY.inventory[@inv_slot] = nil
    PLAYER_INVENTORY.update_slot @inv_slot
  end

  def click_equip(e)
    owner.parent.raise_event :quick_equip, @inv_slot

    if e && e.button == MouseState::RBUTTON
      Event.raise_event :player_drop
    end
  end

  register_component "HotbarComponent"
end

def forward_to_hotbar(ent, ev)
  while ent && !ent.hotbar_component
    ent = ent.parent
  end
  return unless ent
  ent.local_event :click_equip, ev
end
