/*********************************
 * Event.cpp
 * Connor Hilarides
 * Created 2014/08/11
 *********************************/

#include "Common.h"
#include "Helpers\CriticalSections.h"

namespace Events
{
  EventDispatcher *Event::globalDispatcher;

  event_id Event::createEventId(const std::string& name)
  {
    THREAD_EXCLUSIVE_SCOPE;

    static flat_map<std::string, event_id> events;
    static int static_id = 0;

    // Check if an ID record already exists
    auto iter = events.find(name);
    if (iter != events.end())
      return iter->second;

    // Make a new ID record
    auto id = ++static_id;
    events[name] = id;

    return id;
  }


  void Event::raise(EventMessage& e, EventReciever& reciever)
  {
    if (reciever.canHandle(e))
      reciever.handle(e);
  }

}

