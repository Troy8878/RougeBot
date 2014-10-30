#######################
# HotbarComponent.rb
# Jake Robsahm
# Created 2014/10/30
#######################

class HotbarComponent < ComponentBase
  def initialize(data)
    super data

    @target_name = data["target_name"]
    @inv_slot = data["slot"].to_i

    register_event :update, :first_update
  end

  def first_update(e)
    remove_event :update

    @target = find_entity(@target_name)
    @target.proxy_event :inventory_update, self, :on_update
  end

  def remove_item
    holder = self.owner.local_find "ChildHolder"
    return if holder.children.empty?
    
    holder.remove_child holder.children.last
  end

  def display_item(item)
    return if item.nil?

    holder = self.owner.local_find "ChildHolder"
    holder.add_child item.view.create_entity
  end

  def on_update(args)
    return unless args.length == 3
    return unless args[0] == :slot
    return unless args[1] == @inv_slot

    remove_item
    display_item args[2]
  end

  register_component "HotbarComponent"
end
