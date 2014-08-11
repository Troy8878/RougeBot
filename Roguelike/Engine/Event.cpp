/*********************************
 * Event.cpp
 * Connor Hilarides
 * Created 2014/08/11
 *********************************/

#include "Common.h"

event_id Events::EventMessage::createEventId(const std::string& name)
{
  static std::unordered_map<std::string, event_id> events;

  auto iter = events.find(name);
  if (iter != events.end())
    return iter->second;

  auto id = events.size();
  events[name] = id;

  return id;
}

