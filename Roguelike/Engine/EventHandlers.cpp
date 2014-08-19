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

  bool BasicEventDispatcher::CanHandle(const EventMessage& e)
  {
    bool any = false;

    for (auto& hpair : recievers)
    {
      hpair.first = hpair.second->CanHandle(e);
      if (hpair.first)
        any = true;
    }

    return any;
  }

// ----------------------------------------------------------------------------

  void BasicEventDispatcher::Handle(EventMessage& e)
  {
    for (auto& hpair : recievers)
    {
      if (!hpair.first)
        continue;

      hpair.second->Handle(e);
      if (e.Handled)
        return;
    }
  }

// ----------------------------------------------------------------------------

  void BasicEventDispatcher::AddListener(EventReciever *reciever)
  {
    recievers.push_back({false, reciever});
  }

// ----------------------------------------------------------------------------

  void BasicEventDispatcher::RemoveListener(EventReciever *reciever)
  {
    for (auto it = recievers.begin(); it != recievers.end(); ++it)
    {
      if (it->second != reciever)
        continue;

      recievers.erase(it);
      return;
    }
  }

// ----------------------------------------------------------------------------

}
