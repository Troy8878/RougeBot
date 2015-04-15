
GAME_STATE = {}

GAME_STATE[:floor] = 1

def play_dungeon!(start = 1, tut = -1)
  GAME_STATE[:tutorial] = tut
  GAME_STATE[:floor] = start
  Game.switch_level 'Game'
end
