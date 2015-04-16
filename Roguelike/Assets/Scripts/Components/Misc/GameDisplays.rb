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
    score = "Scrap #{GAME_STATE[:score]}"

    maxfloor = GAME_STATE[:endless] ? "" : " / #{$DungeonLength}"
    floor = "Floor #{GAME_STATE[:floor]}#{maxfloor}\nGet to the stairs!"
    
    message = "#{score}\n#{floor}"
    owner.raise_event :send, [:set_text_at, [0, message]]
  end
end

def display_final_score!
  texts = find_entity("Score!").text_component.texts

  GAME_STATE[:score] += 50 # Completion bonus

  scoremsg = "Your final scrap collection is #{GAME_STATE[:score]} units"
  profitmsg = "Company profit is $#{GAME_STATE[:score] - 20}"
  lolgg = "Your share is $20 and a free oil change"
  texts[0] = "#{scoremsg}\n#{profitmsg}\n#{lolgg}"

  GAME_STATE[:score] = 0
end
