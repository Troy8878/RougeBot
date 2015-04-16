#######################
# AiScriptComponent.rb
# Connor Hilarides
# Created 2015/03/16
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class AiScriptComponent < ComponentBase
  include Actor

  attr_reader :brains
  attr_accessor :brain_index, :target

  dependency "AiComponent", "PositionComponent"

  serialized_input do |p|
    p.dependency "AiComponent"
    p.dependency "PositionComponent"

    p.array   :brains, contained: :string, default: []
    p.integer :brain_index, default: 0
    p.string  :target, semantics: :entity, default: "Player"
    p.bool    :auto_exec, default: false
    p.raw     :params, default: {}
  end

  def initialize(data)
    super data

    @brains = data["brains"] || []
    @brain_index = data.fetch("brain_index", 0)
    @target = data.fetch("target", "Player")
    @params = data.fetch("params", {})

    @ai = owner.ai_component
    @pos = owner.position_component
    @pending = false
    @is_kill = false

    register_event :ai_complete, :ai_complete
    register_event :update, :first_update

    if data.fetch("auto_exec", false)
      register_event :logic_update, :logic_update
    end

    register_event :ai_is_kill, :on_kill
  end

  def first_update(e)
    actor_sub_init
    remove_event :update
  end

  def logic_update(e)
    tick
  end

  def tick
    puts "WARNING: double-dipping AI actions" if @pending

    current_target = find_target
    return unless current_target

    @pending = true
    @ai.enqueue brains[brain_index], owner, current_target, @params
  end

  def find_target
    if target.is_a? String
      return find_entity(target)
    end

    target
  end

  def ai_complete(result)
    return if @is_kill

    @pending = false

    case result["result"]
    when "move"

      tile = current_floor[current_floor.size - 1 - result["y"]][result["x"]]
      if !tile.actor
        if brains[brain_index] == "EdgeLord"
          SFX::EDGE_WALK.play
        end

        self.owner.local_event :actor_move, [result["x"], result["y"]]
      end

    when "attack"

      floor = current_floor
      tile = floor[floor.length - 1 - result["y"]][result["x"]]
      if tile.actor
        self.owner.attack_component.do_attack tile.actor
      end

    when nil
      # do nothing
    else
      message = nil

      begin
        result = JSON.parse result["result"]
        
        case result["action"]
        when "message"
          message = result["text"]
          
        when "colorize"
          message = false#"I am red with anger"

          sprite = owner.local_find("Sprite").sprite_component
          old_color = sprite.tint.dup
          sprite.tint = Vector.from_color(result["color"])
          seq = owner.action_sequence :colorize
          seq.delay 0.65
          seq.once do
            sprite.tint = old_color
          end

          #TODO: Colorize for a specific number of turns
          
        when "attack-move"
          self.owner.local_event :actor_move, [result["x"], result["y"]]
          
          floor = current_floor
          tile = floor[floor.length - 1 - result["ty"]][result["tx"]]
          if tile.actor
            self.owner.attack_component.do_attack tile.actor
          end
          
        when "heal"
          floor = current_floor
          tile = floor[floor.length - 1 - result["y"]][result["x"]]
          if tile.actor
            tile.actor.defense_component.heal result["value"]
          end
        end

      rescue Exception => ex
        puts "Error Custom event!"
        puts "#{ex.inspect}"
        ex.backtrace.each do |line|
          puts "  at #{line}"
        end
        message = "failed to interpret hivemind action"
      end

      if message == nil
        message = StatusMessage.new message, 1, "Red", 20, 3
        message.display owner
      end
    end
  end

  def on_kill(e)
    @is_kill = true
  end

  register_component
end
