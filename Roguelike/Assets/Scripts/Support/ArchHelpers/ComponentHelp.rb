#########################
# ComponentHelp.rb
# Connor Hilarides
# Created 2014/09/07
# Copyright © 2015 DigiPen Institute of Technology, All Rights Reserved
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

class Module
  attr_reader :mod_deps

  def module_dependency(*deps)
    @mod_deps ||= []
    deps.each do |dep|
      @mod_deps << dep unless @mod_deps.include? dep
    end
  end
end

class ComponentBase
  @@component_dependencies = []

  def self.dependency(*deps)
    deps.each do |dep|
      @@component_dependencies << dep unless @@component_dependencies.include? dep
    end
  end

  def self.flush_dependencies
    ary = @@component_dependencies
    @@component_dependencies = []
    ary
  end

  def self.include(included_module)
    deps = included_module.mod_deps
    dependency(*deps) if deps
    super included_module
  end
end

