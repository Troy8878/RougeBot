
GAME_STATE = {}

GAME_STATE[:floor] = 1

def play_dungeon!(start = 1)
  GAME_STATE[:floor] = start
  PLAYER_INVENTORY.clear!
  Game.switch_level 'Game'
end
