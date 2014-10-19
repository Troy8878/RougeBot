#######################
# FloorGeneratorComponent.rb
# Connor Hilarides
# Created 2014/09/09
#######################

class FloorGeneratorComponent < ComponentBase
  attr_accessor :floor

  def initialize(data)
    super data

    @@instance = self

    generate_room

    register_event :update, :first_update
  end

  def first_update(e)
    ent = find_entity("Pancake")

    # Set the movement target
    pcc = ent.player_controller_component
    ppos = pcc.instance_variable_get :@pos
    ppos.x = @px
    ppos.z = @py

    # Start the actual position at the beginning
    trc = ent.transform_component
    tpos = trc.position
    tpos.x = @px
    tpos.z = @py

    remove_event :update
  end

  def self.instance
    @@instance
  end

  register_component "FloorGeneratorComponent"
end

def current_floor
  FloorGeneratorComponent.instance.floor
end
