
GAME_STATE = {}

GAME_STATE[:floor] = 1

def play_dungeon!(start = 1, tut = -1)
  if tut == -1 && !Config[:tutorial_completed]
    tut = 0
  end

  GAME_STATE[:tutorial] = tut
  GAME_STATE[:floor] = start
  Game.switch_level 'Game'
end
