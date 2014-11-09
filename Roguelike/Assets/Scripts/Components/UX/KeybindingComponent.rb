#########################
# KeybindingComponent.rb
# Jake Robsahm
# Created 2014/10/09
#########################

class KeybindingComponent < ComponentBase

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
    section = Config.key_bindings[self.owner.name]
    return unless section.is_a? Hash

    binding = section[e.vkey]
    binding = Config.keybind_actions[binding] if binding.is_a? String
    return unless binding.is_a? Hash

    message = binding[type]
    return unless message.is_a? Array

    puts "Dispatching #{e.vkey.to_s 16} to #{self.owner}"
    self.owner.raise_event message.first, message.second
  end

  register_component "KeybindingComponent"
end
