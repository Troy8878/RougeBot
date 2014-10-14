#######################
# DefaultConfig.rb
# Connor Hilarides
# Created 2014/10/07
#######################

Config.load_defaults do

  Config[:keybind_actions] = {}

  Config[:touch_mode] = false
  Config[:key_bindings] = {}

  ######################
  # Player movement

  Config.bind_action "player_move_up",    { held: [:player_move, [ 0,  1]] }
  Config.bind_action "player_move_left",  { held: [:player_move, [-1,  0]] }
  Config.bind_action "player_move_down",  { held: [:player_move, [ 0, -1]] }
  Config.bind_action "player_move_right", { held: [:player_move, [ 1,  0]] }

  Config.bind_key "Game", [Keys::W, Keys::UP,    Keys::NUMPAD8], "player_move_up"
  Config.bind_key "Game", [Keys::A, Keys::LEFT,  Keys::NUMPAD4], "player_move_left"
  Config.bind_key "Game", [Keys::S, Keys::DOWN,  Keys::NUMPAD2], "player_move_down"
  Config.bind_key "Game", [Keys::D, Keys::RIGHT, Keys::NUMPAD6], "player_move_right"

  ######################
  # Minimap

  Config.bind_action "minimap_toggle", { down: [:minimap_toggle] }
  Config.bind_key "Game", Keys::M, "minimap_toggle"

  ######################

end
