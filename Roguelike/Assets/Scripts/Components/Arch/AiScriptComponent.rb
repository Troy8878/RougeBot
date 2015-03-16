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

    register_event :ai_complete, :ai_complete
    register_event :update, :first_update
  end

  def first_update(e)
    actor_sub_init
    remove_event :update
  end

  def tick
    puts "WARNING: double-dipping AI actions" if @pending

    @pending = true
    @ai.enqueue brains[brain_index], owner, find_target, @params
  end

  def find_target
    if target.is_a? String
      return find_entity(target)
    end

    target
  end

  def ai_complete(result)
    @pending = false

    puts result.to_s

    case result["result"]
    when "move"
      self.owner.local_event :actor_move, [result["x"], result["y"]]
    #when "attack"
    else
      message = StatusMessage.new "hivemind sent unknown code\n#{result}", 1, "Red", 20, 3
      message.display owner
    end
  end

  register_component
end
