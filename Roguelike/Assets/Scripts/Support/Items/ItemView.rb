#######################
# ItemView.rb
# Connor Hilarides
# Created 2014/10/21
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#######################

class ItemView
  attr_reader :texture, :decorations, :owner

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
    end
  end

  def add_decoration(decoration)
    
  end

  def create_entity
    entity = GameEntity.create_entity archetype: "Items/ItemView"
    entity.text_component.texts = [owner.name]

    

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
