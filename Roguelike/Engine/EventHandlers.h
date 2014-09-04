/*********************************
 * EventHandlers.h
 * Connor Hilarides
 * Created 2014/08/09
 *********************************/

#pragma once

#include "Event.h"
#include <unordered_set>

class Game;

namespace Events
{

// ----------------------------------------------------------------------------

  class BasicEventDispatcher : public EventDispatcher
  {
  public:
    bool CanHandle(const EventMessage& e) override;
    void Handle(EventMessage& e) override;

    void AddListener(EventReciever *reciever) override;
    void RemoveListener(EventReciever *reciever) override;

  private:
    std::unordered_map<EventReciever *, bool> recievers;
  };

// ----------------------------------------------------------------------------

  template <typename T>
  class BasicClassEventReciever : public EventReciever
  {
  public:
    typedef BasicClassEventReciever event_base;

    BasicClassEventReciever(T *handler);

    typedef void (T::*event_handler)(EventMessage& e);

    bool CanHandle(const EventMessage& e) override;
    void Handle(EventMessage& e) override;

    void SetHandler(event_id id, event_handler handler);
    void RemoveHandler(event_id id);

    template <typename Derived>
    void SetHandler(event_id id, void (Derived::*handler)(EventMessage& e))
    {
      SetHandler(id, static_cast<event_handler>(handler));
    }

  private:
    T *handler;
    flat_map<event_id, event_handler> handlers;
  };

// ----------------------------------------------------------------------------

  template <typename T>
  BasicClassEventReciever<T>::BasicClassEventReciever(T *handler)
    : handler(handler)
  {
  }

// ----------------------------------------------------------------------------

  template <typename T>
  bool BasicClassEventReciever<T>::CanHandle(const EventMessage& e)
  {
    return handlers.find(e.EventId) != handlers.end();
  }

// ----------------------------------------------------------------------------

  template <typename T>
  void BasicClassEventReciever<T>::Handle(EventMessage& e)
  {
    (handler->*handlers[e.EventId])(e);
  }

// ----------------------------------------------------------------------------

  template <typename T>
  void BasicClassEventReciever<T>::SetHandler(event_id id, event_handler handler)
  {
    handlers[id] = handler;
  }

// ----------------------------------------------------------------------------

  template <typename T>
  void BasicClassEventReciever<T>::RemoveHandler(event_id id)
  {
    if (handlers.find(id) != handlers.end())
      handlers.remove(id);
  }

// ----------------------------------------------------------------------------

}
