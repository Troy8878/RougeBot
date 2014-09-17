#########################
# ChildHierarchy.rb
# Connor Hilarides
# Created 2014/09/16
#########################

class ChildHierarchy < ComponentBase

  def initialize(data)
    super data

    @data = data
    register_event :update, :first_update
  end

  def first_update(e)
    @data["children"].each do |child|
      child.each {|k,v| child[k.to_sym] = v }
      self.owner.create_child(child)
    end

    remove_event :update
    remove_instance_variable :@data
  end

  register_component "ChildHierarchy"
end
