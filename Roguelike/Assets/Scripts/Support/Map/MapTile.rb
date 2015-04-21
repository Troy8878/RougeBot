#########################
# MapTile.rb
# Connor Hilarides
# Created 2014/10/08
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#########################

class MapTile
  attr_reader :x, :y, :type_id, :item
  attr_accessor :actor, :projectile, :item_entity, :door

  TYPE_DATA = {
    0 => {
      name: "Floor",
      solid: false
    },
    1 => {
      name: "Wall",
      solid: true
    }
  }

  def initialize(x, y, id)
    @x = x
    @y = y
    @type_id = id
    @type_data = TYPE_DATA[id]
  end

  def name
    @type_data[:name]
  end

  def solid?
    @type_data[:solid]
  end

  def item=(val)
    @item = val
  end

  def item?
    !@item.nil?
  end

  def actor?
    !@actor.nil?
  end

  def projectile?
    !@projectile.nil?
  end

  def drop_item(ditem)
    if self.item?
      pickup_item
    end

    self.item = ditem
    view = ditem.view.create_entity
    view.transform_component.position.x = self.x
    view.transform_component.position.y = current_floor.length - 1 - self.y
    self.item_entity = view
  end

  def pickup_item
    self.item_entity.zombify!
    self.item_entity = nil

    ditem = self.item
    self.item = nil
    ditem
  end
end
