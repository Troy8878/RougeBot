#########################
# ActionManager.rb
# Connor Hilarides
# Created 2014/10/25
# Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
#########################

class ActionManager
  def once(&block)
    self.enqueue do |dt|
      block.call(dt)
      nil
    end
  end
end
