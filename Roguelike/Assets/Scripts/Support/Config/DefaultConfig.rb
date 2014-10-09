#######################
# DefaultConfig.rb
# Connor Hilarides
# Created 2014/10/07
#######################

Config.begin_defaults

Config[:touch_mode] = false
Config[:key_bindings] = {
  "Game" => {
    ['W', Keys::UP,    Keys::NUMPAD8] => :player_move_up,
    ['A', Keys::LEFT,  Keys::NUMPAD4] => :player_move_left,
    ['S', Keys::DOWN,  Keys::NUMPAD2] => :player_move_down,
    ['D', Keys::RIGHT, Keys::NUMPAD6] => :player_move_right,
  }.flatten_keys
}

Config.end_defaults
