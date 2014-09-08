#########################
# DebugHelp.rb
# Connor Hilarides
# Created 2014/09/07
#########################

def find_entity(*args)
  CURRENT_LEVEL.root_entity.find_entity(*args)
end

def search_entities(*args)
  CURRENT_LEVEL.root_entity.search_entities(*args)
end
