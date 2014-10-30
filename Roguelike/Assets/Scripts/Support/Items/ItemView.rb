#######################
# ItemView.rb
# Connor Hilarides
# Created 2014/10/21
#######################

class ItemView
  attr_reader :texture, :color, :decorations, :owner

  def initialize(data, owner = nil)
    @owner = owner

    if data.nil?
      # Set defaults
      @texture = nil
      @color = "White"
      @decorations = []
    elsif data.is_a? ItemView
      # Copy values
      @owner = data.owner
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
        ItemDecoration.new dec_data
      end
    end
  end

  def create_entity
    entity = GameEntity.create_entity archetype: "Items/ItemView"
    entity.text_component.texts = [owner.name]

    #entity.sprite_component.tint = @color
    #self.load_textures entity
    #self.construct_decorations entity

    entity
  end

  def load_textures(entity)
    # TODO
  end

  def construct_decorations(entity)
    # TODO
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
