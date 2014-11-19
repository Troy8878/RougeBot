#######################
# GraphicsConfig.rb
# Connor Hilarides
# Created 2014/11/19
#######################

class Graphics
  def self.load_config
    Config.load
    Graphics.vsync = Config[:graphics_vsync]
  end

  def self.save_config
    Config[:graphics_vsync] = Graphics.vsync
    Config.save
  end
end

Config.hook(:graphics_vsync) do |value|
  Graphics.vsync = value
end

Graphics.vsync = Config[:graphics_vsync] || false
