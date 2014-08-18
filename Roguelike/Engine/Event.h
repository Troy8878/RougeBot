/*********************************
 * Event.h
 * Connor Hilarides
 * Created 2014/06/28
 *********************************/

#pragma once

#include "Common.h"

// ----------------------------------------------------------------------------

typedef ULONG_PTR event_id;

// ----------------------------------------------------------------------------

namespace Events
{
  struct EventData;

// ----------------------------------------------------------------------------

  class EventMessage
  {
    EventData *data;
    event_id _eventId;
#ifdef _DEBUG
    bool _handleable = true;
#endif
    bool _handled = false;

  public:
#ifdef _DEBUG
    EventMessage(event_id eventId, EventData *data, bool handleable = true)
      : data(data), _eventId(eventId), _handleable(handleable)
    {
    }
#else
    inline EventMessage(event_id eventId, EventData *data, bool handleable = true)
      : data(data), _eventId(eventId)
    {
      UNREFERENCED_PARAMETER(handleable);
    }
#endif

    EventData *getData()
    {
      return data;
    }

    template <typename T>
    T *getData()
    {
      return static_cast<T *>(getData());
    }

    inline event_id eventId() const { return _eventId; }

    inline void handled(bool handled) 
    { 
#ifdef _DEBUG
      if (handled && !_handleable)
        throw std::exception("Tried to set that an event was handled on an event that cannot be handled");
#endif
      _handled = handled; 
    }
    inline bool handled() const { return _handled; }
  };

// ----------------------------------------------------------------------------

  class EventReciever abstract
  {
  public:
    virtual bool canHandle(const EventMessage& e) = 0;
    virtual void handle(EventMessage& e) = 0;

  protected:
    inline ~EventReciever() {}
  };

  class EventDispatcher abstract : public EventReciever
  {
  public:
    virtual void addListener(EventReciever *reciever) = 0;
    virtual void removeListener(EventReciever *reciever) = 0;

    static std::unique_ptr<EventDispatcher> createDefaultDispatcher();

  protected:
    inline ~EventDispatcher() {}
  };

// ----------------------------------------------------------------------------

  class Event
  {
  public:
    Event() {}

    static event_id createEventId(const std::string& name);
    static void raise(EventMessage& e, EventReciever& reciever = *Event::globalDispatcher);

    static EventDispatcher *globalDispatcher;
    static EventMessage& currentEvent();

  private:
    static void pushEvent(EventMessage& e);
    static void popEvent();
  };

// ----------------------------------------------------------------------------

}
