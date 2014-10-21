#########################
# ChildHierarchy.rb
# Connor Hilarides
# Created 2014/09/16
#########################

class ChildHierarchy < ComponentBase

  def initialize(data)
    super data

    data["children"].each do |child|
      child.each {|k,v| child[k.to_sym] = v }
      self.owner.create_child(child)
    end
  end

  register_component "ChildHierarchy"
end
