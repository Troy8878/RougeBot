#########################
# ButtonHelperComponent.rb
# Connor Hilarides
# Created 2014/11/19
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

class ButtonHelperComponent < ComponentBase

  def initialize(data)
    super data
    build_components data
  end

  private
  def build_components(data)
    build_button data
    build_background data

    if data.has_key? "image"
      @image = true
      build_image data
    else
      @image = false
      build_text data
    end

    register_event :mouse_enter, :mouse_enter
    register_event :mouse_leave, :mouse_leave
  end

  def mouse_enter(e)
    @bg.sprite_component.tint = @hover_bg
    @txt.text_component.text_color = @text_alt_color unless @image
  end

  def mouse_leave(e)
    @bg.sprite_component.tint = @normal_bg
    @txt.text_component.text_color = @text_color unless @image
  end

  private
  def build_button(data)
    owner.add_component "ButtonComponent", {
      "render_target" => data["render_target"],
      "size" => [data.fetch("width", 3).to_f, 1]
    }
  end

  private
  def build_background(data)
    @normal_bg = Vector.from_color data.fetch("bg_color", "White, 0.6")
    @hover_bg = Vector.from_color(data["bg_hover"] || @normal_bg * Vector.scalar3(0.6))

    @bg = owner.create_child
    @bg.add_component "TransformComponent", {
      "scale" => [data.fetch("width", 3).to_f, 1, 1]
    }
    @bg.add_component "TextureComponent", {
      "textures" => ["SPECIAL/NULL"]
    }
    @bg.add_component "SpriteComponent", {
      "render_target" => data["render_target"],
      "tint" => @normal_bg.to_hex_color
    }
  end

  private
  def build_image(data)
    width = data.fetch("width", 3).to_f
    text_scale = data.fetch("text_scale", 48).to_f

    tex_height = text_scale * 1.5
    tex_width = tex_height * width

    @txt = owner.create_child
    @txt.add_component "TransformComponent", {
      "scale" => [width * 0.95, 0.95, 1]
    }
    @txt.add_component "TextureComponent", {
      "textures" => [data["image"]]
    }
    @txt.add_component "SpriteComponent", {
      "render_target" => data["render_target"]
    }
  end

  private
  def build_text(data)
    width = data.fetch("width", 3).to_f
    text_scale = data.fetch("text_scale", 48).to_f
    @text_color = Vector.from_color data.fetch("text_color", "White")
    @text_alt_color = Vector.from_color(data["text_alt_color"] || @text_color)

    tex_height = text_scale * 1.5
    tex_width = tex_height * width

    @txt = owner.create_child
    @txt.add_component "TransformComponent", {
      "scale" => [width * 0.95, 0.95, 1]
    }
    @txt.add_component "TextureComponent", {
      "textures" => ["SPECIAL/SURFACE/#{tex_width}:#{tex_height}"]
    }
    @txt.add_component "SpriteComponent", {
      "render_target" => data["render_target"]
    }
    @txt.add_component "TextComponent", {
      "font_size" => data.fetch("text_scale", 48).to_i,
      "font" => data.fetch("font", "Segoe UI Symbol"),
      "text_color" => @text_color.to_hex_color,
      "texts" => [data.fetch("text", "<unnamed button>")],
      "text_align" => data.fetch("text_align", "leading")
    }
  end

  register_component
end
