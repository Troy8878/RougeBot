#######################
# Config.rb
# Connor Hilarides
# Created 2014/10/07
#######################

module Config
  CONFIG_DIR = Dir.appdata["DigiPen"]["Roguebot"]
  CONFIG_PATH = CONFIG_DIR.path + "/config.rbd"
  CONFIG_FILE = File.new

  @@defaulting = false
  @@defaults = {}
  @@items = {}
  @@hooks = {}

  def self.save
    file = CONFIG_FILE
    file.open CONFIG_PATH, :out, :truncate
    file.write @@items.inspect
    file.close

    Event.raise_event :config_updated, self
  end

  def self.load
    begin
      self.load_defaults
      file = CONFIG_FILE
      file.open CONFIG_PATH, :in
      new_items = eval(file.read)
      if new_items.is_a? Hash
        @@items.merge! new_items
      else
        log_error "Config wasn't a hash!", new_items
      end
      file.close
    rescue Exception => e
      log_error "An exception occurred reading config file", e
      Config.save
    end
  end

  def self.hook(key, &block)
    @@hooks[key] = block
  end

  def self.[](key)
    @@items[key]
  end

  def self.[]=(key, value)
    if @@defaulting
      @@defaults[key] = value
    end

    @@items[key] = value
    if @@hooks[key]
      @@hooks[key].call value
    end
  end

  def self.is_default?(key)
    @@defaults.has_key?(key) && @@defaults[key] == @@items[key]
  end

  def self.load_defaults(&block)
    @@defaulting = true
    @@defaults = {}
    @@items = {}

    if block_given?
      yield
      @@default_block = block
      Config.load
    else
      @@default_block.call
    end

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

  def self.bound_keys(section, action)
    section = Config.key_bindings[section]
    section.select {|k, v| v == action }.keys
  end
end
