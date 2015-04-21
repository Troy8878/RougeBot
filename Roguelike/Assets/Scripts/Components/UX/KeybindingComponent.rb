#########################
# KeybindingComponent.rb
# Claire Robsahm
# Created 2014/10/09
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#########################

class KeybindingComponent < ComponentBase
  @@locked_input = false

  serialized_input do |p|
  end

  def initialize(data)
    super data

    register_event :key_down, :on_down
    register_event :key_held, :on_held
    register_event :key_up, :on_up
  end

  def on_down(e)
    on_key :down, e
  end

  def on_held(e)
    on_key :held, e
  end

  def on_up(e)
    on_key :up, e
  end

  def on_key(type, e)
    return if @@locked_input

    section = Config.key_bindings[self.owner.name]
    return unless section.is_a? Hash

    binding = section[e.vkey]
    binding = Config.keybind_actions[binding] if binding.is_a? String
    return unless binding.is_a? Hash

    message = binding[type]
    return unless message.is_a? Array

    self.owner.raise_event message.first, message.second
  end

  def self.lock!
    @@locked_input = true
  end

  def self.unlock!
    @@locked_input = false
  end

  register_component "KeybindingComponent"
end
