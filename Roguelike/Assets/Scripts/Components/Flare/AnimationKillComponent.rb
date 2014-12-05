#######################
# AnimationKillComponent.rb
# Connor Hilarides
# Created 2014/09/01
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#######################

class AnimationKillComponent < ComponentBase
  dependency "TransformComponent"

  def initialize(data)
    super data

    position = self.owner.transform_component.position

    seq = owner.action_sequence :status_death
    seq.delay 0.2
    seq.once do
      self.owner.zombify!
    end
  end

  register_component
end
