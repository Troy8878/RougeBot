#######################
# Pancakification.rb
# Connor Hilarides
# Created 2014/10/01
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class Pancakification < ComponentBase

  serialized_input do |p|
  end
  
  def initialize(data)
    super data

    @pancake_face = Texture.load "PancakeFace.jpg"

    register_event :pancakification, :doit
  end

  def doit(e)
    pancakify find_entity(0)
  end

  def pancakify(entity)
    sprite = entity.sprite_component
    unless sprite.nil?
      sprite.tint_texture = @pancake_face
    end
    entity.children.each do |child|
      pancakify child
    end
  end

  register_component "Pancakification"
end
