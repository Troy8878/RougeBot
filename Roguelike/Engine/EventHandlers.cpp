/*********************************
 * EventHandlers.cpp
 * Connor Hilarides
 * Created 2014/08/17
 *********************************/

#include "Common.h"
#include "EventHandlers.h"

namespace Events
{

// ----------------------------------------------------------------------------

  bool BasicEventDispatcher::canHandle(const EventMessage& e)
  {
    bool any = false;

    for (auto& hpair : recievers)
    {
      hpair.first = hpair.second->canHandle(e);
      if (hpair.first)
        any = true;
    }

    return any;
  }

// ----------------------------------------------------------------------------

  void BasicEventDispatcher::handle(EventMessage& e)
  {
    for (auto& hpair : recievers)
    {
      if (!hpair.first)
        continue;

      hpair.second->handle(e);
      if (e.handled())
        return;
    }
  }

// ----------------------------------------------------------------------------

  void BasicEventDispatcher::addListener(EventReciever& reciever)
  {
    recievers.push_back({false, &reciever});
  }

// ----------------------------------------------------------------------------

  void BasicEventDispatcher::removeListener(EventReciever& reciever)
  {
    for (auto it = recievers.begin(); it != recievers.end(); ++it)
    {
      if (it->second != &reciever)
        continue;

      recievers.erase(it);
      return;
    }
  }

// ----------------------------------------------------------------------------

}
