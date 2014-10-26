#########################
# ActionManager.rb
# Connor Hilarides
# Created 2014/10/25
#########################

class ActionManager
  def once(&block)
    self.enqueue do |dt|
      block.call(dt)
      nil
    end
  end
end
