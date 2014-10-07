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
      @@defaults[key] = true
    else
      @@defaults[key] = false
    end

    @@items[key] = value
  end

  def is_default?(key)
    @@defaults.has_key?(key) and @@defaults[key]
  end

  def self.begin_defaults
    @@defaulting = true
  end

  def self.end_defaults
    @@defaulting = false
  end

  def self.each(&block)
    @@items.each do |*args|
      block.call(*args)
    end
  end

  class << self
    include Enumerable
  end
end
