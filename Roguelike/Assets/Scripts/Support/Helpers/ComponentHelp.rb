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

    super *args
  end
end

