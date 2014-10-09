#######################
# Config.rb
# Connor Hilarides
# Created 2014/10/07
#######################

module Config
  @@defaulting = false
  @@defaults = {}
  @@items = {}

  def self.[](key)
    @@items[key]
  end

  def self.[]=(key, value)
    if @@defaulting
      @@defaults[key] = value
    end

    @@items[key] = value
  end

  def self.is_default?(key)
    @@defaults.has_key?(key) && @@defaults[key] == @@items[key]
  end

  def self.load_defaults(&block)
    @@defaulting = true
    @@defaults = {}
    @@items = {}

    if block_given?
      @@default_block = block
    end

    @@default_block.call()

    @@defaulting = false
  end

  def self.each(&block)
    @@items.each do |*args|
      block.call(*args)
    end
  end

  def self.inspect
    @@items.inspect
  end

  class << self
    include Enumerable
  end

  def self.method_missing(*a)
    Config[a[0]] || super(*a)
  end

  def self.bind_action(name, action)
    Config.keybind_actions[name] = action
  end

  def self.bind_key(section, keys, action)
    keys = [keys] unless keys.is_a? Array

    Config.key_bindings[section] ||= {}

    keys.each do |key|
      key = key.char_code if key.is_a? String
      Config.key_bindings[section][key] = action
    end
  end

  def self.unbind_key(section, keys)
    keys = [keys] unless keys.is_a? Array

    Config.key_bindings[section] ||= {}

    keys.each do |key|
      key = key.char_code if key.is_a? String
      Config.key_bindings[section].delete key
    end
  end
end
