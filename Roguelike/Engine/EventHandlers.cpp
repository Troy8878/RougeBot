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

  bool BasicEventDispatcher::CanHandle(const EventMessage &e)
  {
    bool any = false;

    for (auto &hpair : recievers)
    {
      hpair.second = hpair.first->CanHandle(e);
      if (hpair.second)
        any = true;
    }

    static EventId recieverDestroyedId("event_reciever_destroyed");
    return any || e.EventId == recieverDestroyedId;
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
      if (!hpair.second)
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