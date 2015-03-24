def autoplay
  player = find_entity("Player")
  ai = player.ai_script_component
  ai.target = "Player"

  seq = player.action_sequence :autoplay
  proc = []
  proc[0] = Proc.new do
    AISystem.instance.update_snapshot
    ai.tick
    seq.delay 0.6
    seq.once &(proc[0])
  end
  seq.once &(proc[0])
end