#########################
# CheckboxComponent.rb
# Connor Hilarides
# Created 2014/11/19
#########################

class CheckboxComponent < ComponentBase
  dependency "TransformComponent"

  def initialize(data)
    super data
    build_components data
  end

  private
  def build_components(data)
    build_child data
  end

  private
  def build_texture(data)
    texture_data = {
      "textures" => ["UX/Checkbox.png"]
    }
  end

  private
  def build_child(data)
    @child = owner.create_entity
    build_child_texture data

  end

  private
  def build_child_texture(data)
    text_scale = data.fetch("text_scale", 48).to_i
    text_width = data.fetch("text_width", 3).to_i
    texture_height = text_scale * 2
    texture_width = texture_height * text_width

    texture_data = {
      "textures" => ["SPECIAL/SURFACE/#{texture_width}:#{texture_height}"]
    }
    @child.add_component "TextureComponent", texture_data
  end

  private
  def build_child_sprite(data)
    sprite_data = {
      "render_target" => data["render_target"]
    }
    @child.add_component "SpriteComponent", sprite_data
  end

  register_component
end
