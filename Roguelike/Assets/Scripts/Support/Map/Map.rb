#########################
# Map.rb
# Connor Hilarides
# Created 2014/10/08
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

GENERATE_ENTITIES = []

TUTORIAL_PROGRESSION = [
  "Tutorial1"
]

class Map
  attr_reader :player_start

  def generate(opts = {})
    #GAME_STATE[:tutorial] = -1

    opts[:level] = GAME_STATE[:floor]

    tutnum = GAME_STATE[:tutorial]
    if tutnum.is_a? Fixnum
      dungeon = TUTORIAL_PROGRESSION[tutnum]
      if dungeon.nil? || tutnum == -1
        dungeon = "Hubworld"
      end
      opts[:dungeon] = dungeon
    end

    tiles, px, py, sx, sy, entities = Floor.generate(opts)
    @rows = tiles.map.with_index do |row, y|
      row.map.with_index do |tile, x|
        MapTile.new x, y, tile
      end
    end

    STAIR_POSITION.x = sx
    STAIR_POSITION.y = sy
    GENERATE_ENTITIES.replace entities

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

    def length
      @row.length
    end

    def [](index)
      @row[index]
    end
  end

  def [](index)
    MapRow.new @rows[index]
  end

  def length
    @rows.length
  end

  def each(&block)
    @rows.each do |row|
      block.call(MapRow.new row)
    end
  end

  ############################
end
