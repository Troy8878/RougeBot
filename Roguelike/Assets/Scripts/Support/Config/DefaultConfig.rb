#######################
# DefaultConfig.rb
# Connor Hilarides, Troy
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

  Config.bind_key "Player", [Keys::W,    Keys::UP], "move_up"
  Config.bind_key "Player", [Keys::A,  Keys::LEFT], "move_left"
  Config.bind_key "Player", [Keys::S,  Keys::DOWN], "move_down"
  Config.bind_key "Player", [Keys::D, Keys::RIGHT], "move_right"
  Config.bind_key "Player", [Keys::SPACE         ], "skip_turn"

  # Player ranged weapons firing

  Config.bind_action "fire_up",         { held: [:fire, [ 0,  1]] }
  Config.bind_action "fire_left",       { held: [:fire, [-1,  0]] }
  Config.bind_action "fire_down",       { held: [:fire, [ 0, -1]] }
  Config.bind_action "fire_right",      { held: [:fire, [ 1,  0]] }
  Config.bind_action "fire_up-left",    { held: [:fire, [-1,  1]] }
  Config.bind_action "fire_up-right",   { held: [:fire, [ 1,  1]] }
  Config.bind_action "fire_down-left",  { held: [:fire, [-1, -1]] }
  Config.bind_action "fire_down-right", { held: [:fire, [ 1, -1]] }

  Config.bind_key "Player", [Keys::NUMPAD8], "fire_up"
  Config.bind_key "Player", [Keys::NUMPAD4], "fire_left"
  Config.bind_key "Player", [Keys::NUMPAD2], "fire_down"
  Config.bind_key "Player", [Keys::NUMPAD6], "fire_right"
  Config.bind_key "Player", [Keys::NUMPAD1], "fire_up-left"
  Config.bind_key "Player", [Keys::NUMPAD3], "fire_up-right"
  Config.bind_key "Player", [Keys::NUMPAD7], "fire_down-left"
  Config.bind_key "Player", [Keys::NUMPAD9], "fire_down-right"

  ######################
  # Minimap

  Config.bind_action "minimap_toggle", { down: [:minimap_toggle] }
  Config.bind_key "Game", Keys::M, "minimap_toggle"

  ######################
  # Hotbar quick equip

  (1..9).each do |i|
    Config.bind_action "hotbar_equip_#{i}", { down: [:quick_equip, i - 1] }
    Config.bind_key "Hotbar", Keys::N0 + i, "hotbar_equip_#{i}"
  end

  ######################

end
