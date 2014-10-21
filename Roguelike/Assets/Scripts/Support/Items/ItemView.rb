#######################
# ItemView.rb
# Connor Hilarides
# Created 2014/10/21
#######################

class ItemView
  attr_reader :texture, :color, :decorations

  def initialize(data)
    if data.nil?
      # Set defaults
      @texture = nil
      @color = "White"
      @decorations = []

    elsif data.is_a? ItemView
      # Copy values
      @texture = data.texture
      @color = data.color
      @decorations = data.decorations.map do |dec|
        dec.dup
      end

    else
      # Load from JSON
      @texture = data["texture"]
      @color = data.fetch("color", "White")
      @decorations = data["decorations"].map do |dec_data|

      end

    end
  end

  def load_textures
    raise "I can't load textures yet from ruby, sorry :("
  end

  def create_entity
    entity = Entity.create_entity archetype: "ItemView"
    entity.texture_component.textures = self.load_textures
    entity.sprite_component.tint = @color

    self.construct_decorations entity

    entity
  end

  def dup
    ItemView.new self
  end
end

class ItemDecoration
  def initialize(data)
    # TODO: Initialize decorations
  end

  def dup
    ItemDecoration.new self
  end
end
