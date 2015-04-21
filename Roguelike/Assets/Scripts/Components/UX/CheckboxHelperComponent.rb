#########################
# CheckboxHelperComponent.rb
# Connor Hilarides
# Created 2014/11/19
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#########################

class CheckboxHelperComponent < ComponentBase
  attr_reader :checked

  serialized_input do |p|
    p.string :render_target, semantics: :render_target
    p.bool   :is_checked, required: true, default: false
    
    p.string :box_color, semantics: :color, default: "#AFAFAFFF"
    p.string :alt_color, semantics: :color, default: "#007FCFFF"
    p.string :text_color, semantics: :color, default: "White"

    p.float  :text_width, default: 3
    p.float  :text_scale, default: 48

    p.string :font, semantics: :font
    p.string :text, required: true, default: ""

    p.string :config_bind, default: nil
  end

  def initialize(data)
    super data

    @checked = data.fetch("is_checked", false)
    build_components data

    register_event :button_clicked, :on_click
    register_event :mouse_enter, :mouse_enter
    register_event :mouse_leave, :mouse_leave

    if data.has_key? "config_bind"
      key = data["config_bind"].to_sym
      self.checked = !!Config[key]

      define_singleton_method(:on_checkbox_toggled) do
        Config[key] = checked
        Config.save
      end

      define_singleton_method(:on_config_updated) do
        @checked = !!Config[key]
        update_checkmark
      end

      register_event :checkbox_toggled, :on_checkbox_toggled
      register_event :config_updated, :on_config_updated
    end
  end

  def on_click(e)
    self.checked = !@checked
  end

  def checked=(val)
    @checked = val
    update_checkmark
    owner.raise_event :checkbox_toggled, @checked

    if @checked
      owner.raise_event :checked, @checked
    else
      owner.raise_event :unchecked, @checked
    end
  end

  def mouse_enter(e)
    @borderbox.sprite_component.tint = @alt_color
  end

  def mouse_leave(e)
    @borderbox.sprite_component.tint = @border_color
  end

  def update_checkmark
    @checkmark.sprite_component.visible = @checked
  end

  private
  def build_components(data)
    build_button data
    build_box data
    build_child data

    @checkmark = owner.local_find "Checkmark"
    update_checkmark
  end

  private
  def build_button(data)
    owner.add_component "ButtonComponent", {
      "render_target" => data["render_target"]
    }
    owner.add_component "ButtonSoundsComponent", {}
  end

  private
  def build_box(data)
    @box_color = Vector.string_to_color data.fetch("box_color", "#AFAFAFFF")
    @alt_color = Vector.string_to_color data.fetch("alt_color", "#007FCFFF")
    @border_color = @box_color * Vector.scalar3(0.7)

    @borderbox = owner.create_child components: {
      "TextureComponent" => {
        "textures" => ["SPECIAL/NULL"]
      },
      "SpriteComponent" => {
        "render_target" => data["render_target"],
        "tint" => @border_color.to_hex_color
      },
      "ChildHierarchy" => {
        "children" => [
          {
            "components" => {
              "TransformComponent" => {
                "position" => [0, 0, -0.0025],
                "scale" => [0.9, 0.9, 1]
              },
              "TextureComponent" => {
                "textures" => ["SPECIAL/NULL"]
              },
              "SpriteComponent" => {
                "render_target" => data["render_target"],
                "tint" => @box_color.to_hex_color
              }
            }
          },
          {
            "name" => "Checkmark",
            "components" => {
              "TransformComponent" => {
                "position" => [0, 0, -0.005],
                "scale" => [1, 1, 1]
              },
              "TextureComponent" => {
                "textures" => ["SPECIAL/SURFACE/128:128"]
              },
              "SpriteComponent" => {
                "render_target" => data["render_target"]
              },
              "TextComponent" => {
                "texts" => ["✔"],
                "font_size" => 72,
                "text_color" => "Black"
              }
            }
          }
        ]
      }
    }
  end

  private
  def build_child(data)
    @child = owner.create_child
    build_child_transform data
    build_child_texture data
    build_child_sprite data
    build_child_text data
  end

  private
  def build_child_transform(data)
    text_width = data.fetch("text_width", 5).to_i
    @child.add_component "TransformComponent", {
      "position" => [0.75 + text_width / 2, 0, 0],
      "scale" => [text_width, 1, 1]
    }
  end

  private
  def build_child_texture(data)
    text_scale = data.fetch("text_scale", 48).to_i
    text_width = data.fetch("text_width", 5).to_i
    texture_height = text_scale * 2
    texture_width = texture_height * text_width

    @child.add_component "TextureComponent", {
      "textures" => ["SPECIAL/SURFACE/#{texture_width}:#{texture_height}"]
    }
  end

  private
  def build_child_sprite(data)
    @child.add_component "SpriteComponent", {
      "render_target" => data["render_target"]
    }
  end

  private
  def build_child_text(data)
    @child.add_component "TextComponent", {
      "font_size" => data.fetch("text_scale", 48).to_i,
      "font" => data.fetch("font", "Segoe UI"),
      "text_color" => data.fetch("text_color", "White"),
      "texts" => [data.fetch("text", "<unnamed checkbox>")],
      "text_align" => "leading"
    }
  end

  register_component
end
