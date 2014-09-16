#########################
# ChildHierarchy.rb
# Connor Hilarides
# Created 2014/09/16
#########################

class ChildHierarchy < ComponentBase

  def initialize(data)
    @data = data
    register_event :update, :first_update
  end

  def first_update
    @data["children"].each do |child|
      self.owner.create_child child
    end
  end

  register_component "ChildHierarchy"
end
