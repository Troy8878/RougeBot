#######################
# StatusMessageComponent.rb
# Connor Hilarides
# Created 2014/09/01
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class StatusMessageComponent < ComponentBase
  def initialize(data)
    super data

    position = self.owner.transform_component.position
    delete_owner = data["delete_owner"]

    seq = owner.action_sequence :status_death
    seq.interpolate position, by: Vector.up, over: 1.second
    seq.once do
      if delete_owner
        self.owner.parent.zombify!
      else
        self.owner.zombify!
      end
    end
  end

  register_component
end
