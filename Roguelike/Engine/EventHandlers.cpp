/*********************************
 * EventHandlers.cpp
 * Connor Hilarides
 * Created 2014/08/17
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "EventHandlers.h"

namespace Events
{
  // ----------------------------------------------------------------------------

  bool BasicEventDispatcher::CanHandle(const EventMessage &)
  {
    return true;
  }

  // ----------------------------------------------------------------------------

  void BasicEventDispatcher::Handle(EventMessage &e)
  {
    static EventId recieverDestroyedId("event_reciever_destroyed");

    if (e.EventId == recieverDestroyedId)
    {
      auto rec = e.GetData<EventRecieverDestroyedEvent>()->reciever;
      RemoveListener(rec);
    }

    for (auto &hpair : recievers)
    {
      if (!hpair.first->CanHandle(e))
        continue;

      hpair.first->Handle(e);
      if (e.Handled)
        return;
    }
  }

  // ----------------------------------------------------------------------------

  void BasicEventDispatcher::AddListener(EventReciever *reciever)
  {
    recievers[reciever] = false;
  }

  // ----------------------------------------------------------------------------

  void BasicEventDispatcher::RemoveListener(EventReciever *reciever)
  {
    auto it = recievers.find(reciever);
    if (it == recievers.end())
      return;

    recievers.erase(it);
  }

  // ----------------------------------------------------------------------------
}