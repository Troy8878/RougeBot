#########################
# TransformComponent.rb
# Connor Hilarides
# Created 2014/09/24
#########################

module Components
  class TransformComponent < ComponentBase

    def dup_for_hash
      scl = self.scale
      pos = self.position
      rot = self.rotation

      {
        "scale"    => [scl.x, scl.y, scl.z, scl.w],
        "position" => [pos.x, pos.y, pos.z, pos.w],
        "rotation" => [rot.x, rot.y, rot.z, rot.w]
      }
    end

  end
end
