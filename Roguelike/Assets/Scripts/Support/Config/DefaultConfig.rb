#######################
# DefaultConfig.rb
# Connor Hilarides, Troy
# Created 2014/10/07
#######################

Config.load_defaults do

  Config[:console_enabled] = true

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
  Config.bind_action "fire_ul",         { held: [:fire, [-1,  1]] }
  Config.bind_action "fire_ur",         { held: [:fire, [ 1,  1]] }
  Config.bind_action "fire_dl",         { held: [:fire, [-1, -1]] }
  Config.bind_action "fire_dr",         { held: [:fire, [ 1, -1]] }

  Config.bind_key "Player", [Keys::I,          Keys::NUMPAD8], "fire_up"
  Config.bind_key "Player", [Keys::J,          Keys::NUMPAD4], "fire_left"
  Config.bind_key "Player", [Keys::L,          Keys::NUMPAD2], "fire_down"
  Config.bind_key "Player", [Keys::OEM_COMMA,  Keys::NUMPAD6], "fire_right"
  Config.bind_key "Player", [Keys::U,          Keys::NUMPAD7], "fire_ul"
  Config.bind_key "Player", [Keys::O,          Keys::NUMPAD9], "fire_ur"
  Config.bind_key "Player", [Keys::M,          Keys::NUMPAD1], "fire_dl"
  Config.bind_key "Player", [Keys::OEM_PERIOD, Keys::NUMPAD3], "fire_dr"

  ######################
  # Minimap

  Config.bind_action "minimap_toggle", { down: [:minimap_toggle] }
  Config.bind_key "Game", '/', "minimap_toggle"

  ######################
  # Hotbar quick equip

  (1..9).each do |i|
    Config.bind_action "hotbar_equip_#{i}", { down: [:quick_equip, i - 1] }
    Config.bind_key "Hotbar", (Keys::N0 + i), "hotbar_equip_#{i}"
  end

  ######################

  Config.bind_action "pancakification", { down: [:pancakification] }
  Config.bind_key "Game", Keys::PAUSE, "pancakification"

  Config.bind_action "toggle_console", { down: [:toggle_console] }
  Config.bind_key "Game", Keys::TILDE, "toggle_console"

end
