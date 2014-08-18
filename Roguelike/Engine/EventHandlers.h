/*********************************
 * EventHandlers.h
 * Connor Hilarides
 * Created 2014/08/09
 *********************************/

#pragma once

#include "Event.h"

class Game;

namespace Events
{

// ----------------------------------------------------------------------------

  class BasicEventDispatcher : public EventDispatcher
  {
  public:
    bool canHandle(const EventMessage& e) override;
    void handle(EventMessage& e) override;

    void addListener(EventReciever *reciever) override;
    void removeListener(EventReciever *reciever) override;

  private:
    std::vector<std::pair<bool, EventReciever *>> recievers;
  };

// ----------------------------------------------------------------------------

  template <typename T>
  class BasicClassEventReciever : public EventReciever
  {
  public:
    BasicClassEventReciever(T *handler);

    typedef void (T::*event_handler)(EventMessage& e);

    bool canHandle(const EventMessage& e) override;
    void handle(EventMessage& e) override;

    void setHandler(event_id id, event_handler handler);
    void removeHandler(event_id id);

    template <typename Derived>
    void setHandler(event_id id, void (Derived::*handler)(EventMessage& e))
    {
      setHandler(id, static_cast<event_handler>(handler));
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
  bool BasicClassEventReciever<T>::canHandle(const EventMessage& e)
  {
    return handlers.find(e.eventId()) != handlers.end();
  }

// ----------------------------------------------------------------------------

  template <typename T>
  void BasicClassEventReciever<T>::handle(EventMessage& e)
  {
    (handler->*handlers[e.eventId()])(e);
  }

// ----------------------------------------------------------------------------

  template <typename T>
  void BasicClassEventReciever<T>::setHandler(event_id id, event_handler handler)
  {
    handlers[id] = handler;
  }

// ----------------------------------------------------------------------------

  template <typename T>
  void BasicClassEventReciever<T>::removeHandler(event_id id)
  {
    if (handlers.find(id) != handlers.end())
      handlers.remove(id);
  }

// ----------------------------------------------------------------------------

}
