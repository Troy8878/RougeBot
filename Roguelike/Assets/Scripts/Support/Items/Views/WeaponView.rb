#########################
# FileName.rb
# Connor Hilarides
# Created 2014/12/05
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

class Item
end

class Weapon < Item
  class View
    def initialize(item)
      @item = item
    end

    def create_entity
      base_view = GameEntity.create_entity(
        archetype: "Items/WeaponView",
        components: {
          "TextureComponent" => {
            "textures" => ["Weapons/#{@item.weapon_type_name}.png"]
          }
        }
      )

      add_attributes base_view
      return base_view
    end

    def add_attributes(base_view)
      @item.attributes.each do |attrib|
        func = :"mod_#{attrib.category}"
        if self.respond_to? func
          self.send(func, base_view, attrib.value)
        end
      end
    end

    MATERIAL_TINT = {
      "Diamond"       => Vector.new(0.8, 0.8,  1.0, 0.9),
      "Steel"   => Vector.new(1.0, 1.0,  1.0, 1.0),
      "Obsidian"     => Vector.new(0.1, 0.1, 0.1, 1.0),
      "Lead"    => Vector.new(0.6, 0.6,  0.8, 1.0),
      "Asbestos" => Vector.new(1.6, 1.6,  1.6, 1.0),
      "Iron" => Vector.new(0.8, 0.8,  0.8, 1.0),
      "Plastic" => Vector.new(0.9, 0.9,  0.0, 1.0),
      "Mahogany" => Vector.new(0.3, 0.15,  0.0, 1.0),
      "Alleged" => Vector.new(0.0, 0.0,  0.0, 0.6),
      "Wooden" => Vector.new(0.2, 0.1,  0.0, 1.0),
    }

    def mod_material(base_view, value)
      base_view.sprite_component.tint = MATERIAL_TINT[value]
    end

    def mod_augment(base_view, value)
      tex = Texture.load "Weapons/Augments/#{value}.png"
      return if tex.width == 1

      base_view.sprite_component.tint_texture = tex
    end

    WEAPON_GLOWS = {
      "Flame"       => Vector.new(1.0, 0.6,  0.3, 1.0),
      "Exploding"   => Vector.new(1.0, 1.0,  0.0, 1.0),
      "Zapping"     => Vector.new(0.0, 0.76, 1.0, 1.0),
      "Freezing"    => Vector.new(1.0, 1.0,  1.0, 1.0),
      "Radioactive" => Vector.new(0.0, 1.0,  0.0, 1.0)
    }

    def mod_element(base_view, value)
      return unless WEAPON_GLOWS.include? value

      glow = base_view.create_child(archetype: "Items/WeaponGlow")
      glow.sprite_component.tint = WEAPON_GLOWS[value]
    end
  end
end