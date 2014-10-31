#######################
# Pancakification.rb
# Jake Robsahm
# Created 2014/09/11
#######################

#########################Class Details############################
# A component that contains the inventory of the entity.
#######################Property Details###########################
# inventory - the inventory contained by the component.
##################################################################

class Pancakification < ComponentBase
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
