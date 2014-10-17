#########################
# Map.rb
# Connor Hilarides
# Created 2014/10/08
#########################

class Map
  attr_reader :player_start

  def generate(opts = {})
    tiles, px, py = Floor.generate(opts)
    @rows = tiles.map.with_index do |row, y|
      row.map.with_index do |tile, x|
        MapTile.new x, y, tile
      end
    end

    @player_start = [px, py]
  end

  ############################
  # Enumerable stuff
  include Enumerable

  class MapRow
    include Enumerable

    def initialize(row)
      @row = row
    end

    def each(&block)
      @row.each(&block)
    end

    def [](index)
      @row[index]
    end
  end

  def [](index)
    MapRow.new @rows[index]
  end

  def each(&block)
    @rows.each do |row|
      block.call(MapRow.new row)
    end
  end

  ############################
end
