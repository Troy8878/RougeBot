#########################
# MapTile.rb
# Connor Hilarides
# Created 2014/10/08
#########################

class MapTile
  attr_reader :x, :y, :type_id, :item
  attr_accessor :actor

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
    @item.picked_up if @item
    @item = val
    @item.put_down self if @item
  end

  def actor?
    !@actor.nil?
  end
end
