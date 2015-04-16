#########################
# GameAudio.rb
# Connor Hilarides
# Created 2014/12/05
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

module SFX
  HIGH_CLICK = Sound.load 'SFX/HighClick'
  LOW_TICK = Sound.load 'SFX/LowTick'
  SLASH = Sound.load 'SFX/Slash'
  UGH = Sound.load 'SFX/Ugh'
  PLAYER_WALK = Sound.load 'SFX/Normal_Walk'
  EDGE_WALK = Sound.load 'SFX/EdgeLord_Walk'
  EAT = Sound.load 'SFX/Fire'

  FAILURE = Sound.load 'SFX/You_Have_Failed'
  DUNGEON_CLEAR = Sound.load 'SFX/Dungeon_Clear'

  ITEM_DROP = Sound.load 'SFX/ItemDrop'
  ITEM_PICKUP = Sound.load 'SFX/ItemGet'
end

module MUSIC
  MENU_MUSIC = Sound.load 'Music/Creepy'
  TUT = Sound.load 'Music/Draft_of_the_Ancient_Halls'
  ACT1 = Sound.load 'Music/Act1'
  ACT2 = Sound.load 'Music/Act2'

  @@old_music = nil
  def self.play(music)
    if music == @@old_music
      return
    end

    if @@old_music
      @@old_music.stop
    end

    if music
      music.play
    end

    old = @@old_music
    @@old_music = music
    return old
  end
end

MUSIC.play(MUSIC::MENU_MUSIC)
