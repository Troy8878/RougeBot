#########################
# ComponentHelp.rb
# Connor Hilarides
# Created 2014/09/07
#########################

class GameEntity
  def method_missing(*args)
    name = args[0].to_s
    if args.count == 1 and name =~ /_component$/
      compName = name.split('_').map{|s|s.capitalize}.join
      return get_component compName
    end

    super(*args)
  end

  def create_child(*args)
    entity = GameEntity.create_entity(*args)
    self.add_child entity
  end
end

class ComponentBase
  @@component_dependencies = []

  def self.dependency(*deps)
    @@component_dependencies += deps
  end

  def self.flush_dependencies
    ary = @@component_dependencies
    @@component_dependencies = []
    ary
  end
end

