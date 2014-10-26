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

  Config.bind_action "move_up",    { held: [:move, [ 0,  1]] }
  Config.bind_action "move_left",  { held: [:move, [-1,  0]] }
  Config.bind_action "move_down",  { held: [:move, [ 0, -1]] }
  Config.bind_action "move_right", { held: [:move, [ 1,  0]] }
  Config.bind_action "skip_turn",  { held: [:move, [ 0,  0]] }

  Config.bind_key "Player", [Keys::W, Keys::UP,    Keys::NUMPAD8], "move_up"
  Config.bind_key "Player", [Keys::A, Keys::LEFT,  Keys::NUMPAD4], "move_left"
  Config.bind_key "Player", [Keys::S, Keys::DOWN,  Keys::NUMPAD2], "move_down"
  Config.bind_key "Player", [Keys::D, Keys::RIGHT, Keys::NUMPAD6], "move_right"
  Config.bind_key "Player", [Keys::SPACE                        ], "skip_turn"

  ######################
  # Minimap

  Config.bind_action "minimap_toggle", { down: [:minimap_toggle] }
  Config.bind_key "Game", Keys::M, "minimap_toggle"

  ######################
  # Temp Inventory

  Config.bind_action "set_weapon_1", { down: [:equip_weapon, 0] }

  Config.bind_key "Player", Keys::N1, "set_weapon_1"

  ######################

end
