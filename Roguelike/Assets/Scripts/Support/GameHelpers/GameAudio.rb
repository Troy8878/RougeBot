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
end

module MUSIC
  MENU_MUSIC = Sound.load 'Music/Creepy'

  @@old_music = nil
  def self.play(music)
    if @@old_music
      @@old_music.stop
    end
    music.play
    @@old_music = music
  end
end

MUSIC.play(MUSIC::MENU_MUSIC)
