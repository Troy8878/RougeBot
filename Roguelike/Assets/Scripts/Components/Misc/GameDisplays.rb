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
    owner.raise_event :send, [:set_text_at, [0, ""]]
  else
    score = "Score #{GAME_STATE[:score]}"

    maxfloor = GAME_STATE[:endless] ? "" : " / #{$DungeonLength}"
    floor = "Floor #{GAME_STATE[:floor]}#{maxfloor}\nGet to the stairs!"
    
    message = "#{score}\n#{floor}"
    owner.raise_event :send, [:set_text_at, [0, message]]
  end
end
