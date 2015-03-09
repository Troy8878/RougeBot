#######################
# SerializationInput.rb
# Connor Hilarides
# Created 2015/01/27
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class TransformComponent < ComponentBase
  # Describe what goes in the json file
  serialized_input do |p|
    p.vector :position, dimms: 3, default: [0, 0, 0]
    p.vector :rotation, dimms: 3, default: [0, 0, 0]
    p.vector :scale,    dimms: 3, default: [1, 1, 1]
  end
end

class TextureComponent < ComponentBase
  # Describe what goes in the json file
  serialized_input do |p|
    p.array :textures, contained: :texture, default: []
  end
end

class SpriteComponent < ComponentBase
  # Describe what goes in the json file
  serialized_input do |p|
    p.string :render_target, semantics: :render_target, required: true
    p.string :shader,        semantics: :shader
    p.vector :tint,          semantics: :color, default: [1, 1, 1, 1]
  end
end

class ButtonComponent < ComponentBase
  # Describe what goes in the json file
  serialized_input do |p|
    p.string :render_target, semantics: :render_target, required: true
  end
end

class CameraComponent < ComponentBase
  # Describe what goes in the json file
  serialized_input do |p|
    p.string  :target_name, required: true
    p.integer :layer
    p.string  :copy, semantics: :render_target
    p.string  :type, default: "ManualCamera", locked: true # This is the One True Camera™
  end
end

class CustomModelComponent < ComponentBase
  # Describe what goes in the json file
  serialized_input do |p|
    p.string :shader,        semantics: :shader
    p.string :render_target, semantics: :render_target
  end
end

class ParticleSystemComponent < ComponentBase
  # Describe what goes in the json file
  serialized_input do |p|
    p.string :render_target, semantics: :render_target, required: true
    p.vector :scaleRange
    p.vector :rotationRange
    p.vector :velocityRange
    p.vector :rotVelRange
    p.vector :offSet
    p.float  :particleRate
    p.float  :fadeTime
    p.bool   :active
    p.string :texture, semantics: :texture
  end
end

class TextComponent < ComponentBase
  # Describe what goes in the json file
  serialized_input do |p|
    p.array  :texts,      contained: :string
    p.string :text_color, semantics: :color
    p.string :bg_color,   semantics: :color
    p.string :font,       semantics: :font
    p.float  :font_size,  default: 48
    p.enum   :text_align, options: ["center", "justified", "leading", "trailing"]
    p.enum   :paragraph_align, options: ["center", "near", "far"]
    p.vector :autocreate_textures, dimms: 2
  end
end

class PositionComponent < ComponentBase
  serialized_input do |p|
    p.float  :move_speed, default: 5
    p.vector :position,   dimms: 2
    p.enum   :mode,       required: true, options: ["jump", "linear", "exponential"]
  end
end
