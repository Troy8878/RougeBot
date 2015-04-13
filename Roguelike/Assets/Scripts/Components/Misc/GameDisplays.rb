def run_combat_log(e)
  seq = owner.action_sequence :log
  seq.once do
    msg = owner.create_child(archetype: 'HUD/LogMessage')
    msg.text_component.texts[0] = e.to_s
  end
  seq.delay 0.4
end

def init_floor_text(entity)
  if !GAME_STATE[:tutorial].nil?
    puts "u wot"
    owner.raise_event :send, [:set_text_at, [0, ""]]
  else
    message = "Floor #{GAME_STATE[:floor]} / #{$DungeonLength}\nGet to the stairs!"
    owner.raise_event :send, [:set_text_at, [0, message]]
  end
end
