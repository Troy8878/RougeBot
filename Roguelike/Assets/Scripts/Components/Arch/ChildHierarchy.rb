#########################
# ChildHierarchy.rb
# Connor Hilarides
# Created 2014/09/16
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
#########################

class ChildHierarchy < ComponentBase

  serialized_input do |p|
    p.array :children, contained: :entity, required: true
  end

  def initialize(data)
    super data

    data["children"].each do |child|
      child.each {|k,v| child[k.to_sym] = v }
      self.owner.create_child(child)
    end
  end

  register_component "ChildHierarchy"
end
