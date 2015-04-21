#######################
# AHelpfulLibrary.rb
# Connor Hilarides
# Created 2014/09/09
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#######################

module Kernel
  def method(sym)
    Proc.new {|*args| self.send sym, *args }
  end
end

module Math
  def Math.signum(i)
    -1 if i < 0
    1 if i > 0
    0
  end

  def Math.round(i)
    return (i + 0.5).to_i if i >= 0
    return (i - 0.5).to_i
  end

  def Math.abs(i)
    i < 0 ? -i : i
  end
end

module Enumerable
  def size
    length
  end

  def count
    length
  end
end

class Array
  def sum
    total = first
    self[1..-1].each do |item|
      total += item
    end
    total
  end

  def average
    sum / count
  end

  def rand_item
    return nil if empty?

    self[(rand * length).to_i]
  end
end

module Enumerable
  def init
    lazy.take(count - 1).to_a
  end

  def tail
    lazy.drop(1).to_a
  end

  def enum_at(i)
    self.each do |item|
      return item if i == 0
      i -= 1
    end
    nil
  end

  def first
    enum_at 0
  end

  def second
    enum_at 1
  end

  def third
    enum_at 2
  end

  def fourth
    enum_at 3
  end

  def fifth
    enum_at 4
  end
end

class String
  def char_code
    bytes[0]
  end
end

class Symbol
  def to_proc
    -> (a1, *args) do
      a1.send self, *args
    end
  end
end

module Kernel
  ERROR_LOG_DIR = Dir.appdata["DigiPen"]["Roguebot"]["GameLogs"]
  ERROR_LOG_PATH = ERROR_LOG_DIR.path + "/error.log"
  ERROR_LOG_FILE = File.new

  def log_error(msg, *args)
    ERROR_LOG_FILE.open ERROR_LOG_PATH, :out, :append
    ERROR_LOG_FILE.write "#{msg}: #{args.map(&:inspect).join ', '}\n"
    ERROR_LOG_FILE.close
  end
end

###########
# Merging

class Hash
  def merge!(other)
    raise TypeError, "Types must match for merging!" unless other.is_a? Hash
    other.each do |key, value|
      if self.has_key? key
        if self[key].is_a? value.class
          if self[key].respond_to? :merge!
            self[key].merge! value
            next
          end
        end
      end
      self[key] = value
    end
  end
end

def load_wall_of_text(asset, entity)
  asset = Asset.load 'WallsOfText', asset
  entity.text_component.texts[0] = asset.read_text
end
