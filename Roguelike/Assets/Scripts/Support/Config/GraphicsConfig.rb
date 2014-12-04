#######################
# GraphicsConfig.rb
# Connor Hilarides
# Created 2014/11/19
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class Graphics

  HOOKS = [
    :vsync,
    :fullscreen
  ]

  def self.load_config
    Config.load

    HOOKS.each do |hook|
      Graphics.send(:"#{hook}=", Config[:"graphics_#{hook}"])
    end
  end

  def self.save_config
    HOOKS.each do |hook|
      Config.send(:[]=, :"graphics_#{hook}", Graphics.send(hook))
    end

    Config.save
  end

  HOOKS.each do |hook|
    Config.hook(:"graphics_#{hook}") do |value|
      Graphics.send(:"#{hook}=", value)
    end

    Graphics.send(:"#{hook}=", Config[:"graphics_#{hook}"] || Graphics.send(hook))
  end
end
