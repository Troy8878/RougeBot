#######################
# DefaultConfig.rb
# Connor Hilarides, Troy
# Created 2014/10/07
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

Config.load_defaults do

  Config[:touch_mode] = false
  Config[:console_enabled] = false

  ######################
  # Graphics

  Config[:graphics_vsync] = true
  Config[:graphics_fullscreen] = true

  ######################
  # Key Bindings

  Config[:keybind_actions] = {}
  Config[:key_bindings] = {}

  ######################
  # Player movement

  Config.bind_action "move_up",     { held: [:move,  [ 0,  1] ] }
  Config.bind_action "move_left",   { held: [:move,  [-1,  0] ] }
  Config.bind_action "move_down",   { held: [:move,  [ 0, -1] ] }
  Config.bind_action "move_right",  { held: [:move,  [ 1,  0] ] }
  Config.bind_action "skip_turn",   { held: [:move,  [ 0,  0] ] }
  Config.bind_action "player_use",  { down: [:player_use, nil ] }
  Config.bind_action "player_drop", { down: [:player_drop, nil] }

  # Some Cheats
  Config.bind_action "skip_floor", { down: [:skip_floor, nil] }
  Config.bind_action "skip_to_win", { down: [:skip_to_win, nil] }

  Config.bind_key "Player", [Keys::W    ], "move_up"
  Config.bind_key "Player", [Keys::A    ], "move_left"
  Config.bind_key "Player", [Keys::S    ], "move_down"
  Config.bind_key "Player", [Keys::D    ], "move_right"
  Config.bind_key "Player", [Keys::SPACE], "skip_turn"
  Config.bind_key "Player", [Keys::E    ], "player_use"
  Config.bind_key "Player", [Keys::Q    ], "player_drop"

  # Some Cheats
  Config.bind_key "Player", [Keys::N    ], "skip_floor"
  Config.bind_key "Player", [Keys::B    ], "skip_to_win"

  # Player melee weapon firing

  Config.bind_action "attack_up",    { held: [:swing_weapon, :up] }
  Config.bind_action "attack_left",  { held: [:swing_weapon, :left] }
  Config.bind_action "attack_down",  { held: [:swing_weapon, :down] }
  Config.bind_action "attack_right", { held: [:swing_weapon, :right] }

  Config.bind_key "Player", [Keys::UP   ], "attack_up"
  Config.bind_key "Player", [Keys::LEFT ], "attack_left"
  Config.bind_key "Player", [Keys::DOWN ], "attack_down"
  Config.bind_key "Player", [Keys::RIGHT], "attack_right"

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
  Config.bind_key "Player", [Keys::OEM_COMMA,  Keys::NUMPAD2], "fire_down"
  Config.bind_key "Player", [Keys::L,          Keys::NUMPAD6], "fire_right"
  Config.bind_key "Player", [Keys::U,          Keys::NUMPAD7], "fire_ul"
  Config.bind_key "Player", [Keys::O,          Keys::NUMPAD9], "fire_ur"
  Config.bind_key "Player", [Keys::M,          Keys::NUMPAD1], "fire_dl"
  Config.bind_key "Player", [Keys::OEM_PERIOD, Keys::NUMPAD3], "fire_dr"

  ######################
  # Minimap

  Config.bind_action "minimap_toggle", { down: [:minimap_toggle] }
  Config.bind_key "Game", Keys::TAB, "minimap_toggle"

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