#########################
# Map.rb
# Connor Hilarides
# Created 2014/10/08
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

GENERATE_ENTITIES = []

TUTORIAL_PROGRESSION = [
  "Tutorial1",
  "Tutorial2",
  "Tutorial3"
]

class Map
  attr_reader :player_start

  def generate(opts = {})
    opts[:level] = GAME_STATE[:floor]
    tutnum = GAME_STATE[:tutorial]

    if tutnum == nil && Config[:act2] != true
      MUSIC.play(MUSIC::ACT1)
    end

    if tutnum == -3
      MUSIC.play(MUSIC::ACT2)
      GAME_STATE[:tutorial] = nil
      GAME_STATE[:act2] = true
      GAME_STATE[:endless] = false
      tutnum = nil
    elsif tutnum == -4
      MUSIC.play(MUSIC::ACT2)
      GAME_STATE[:tutorial] = nil
      GAME_STATE[:act2] = true
      GAME_STATE[:endless] = true
      tutnum = nil
    elsif tutnum == -1
      MUSIC.play(MUSIC::TUT)
      GAME_STATE[:act2] = false
      GAME_STATE[:endless] = false
      GAME_STATE[:phealth] = 100.to_f
    end

    if tutnum.is_a? Fixnum
      dungeon = TUTORIAL_PROGRESSION[tutnum]
      if dungeon.nil? || tutnum == -1
        if tutnum >= TUTORIAL_PROGRESSION.length
          Config[:tutorial_completed] = true
          Config.save
        end
        if tutnum == -5
          dungeon = "Outro"
        else
          GAME_STATE[:tutorial] = -1
          dungeon = "Hubworld"
        end
      else
        MUSIC.play(MUSIC::TUT)
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
