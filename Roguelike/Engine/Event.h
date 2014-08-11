/*********************************
 * Event.h
 * Connor Hilarides
 * Created 2014/06/28
 *********************************/

#pragma once

#include "Common.h"

typedef ULONG_PTR event_id;

namespace Events
{
  class EventMessage
  {
    void *data;
    event_id _eventId;
#ifdef _DEBUG
    std::type_index _event_type;
    bool _handleable = true;
#endif
    bool _handled = false;

  public:
#ifdef _DEBUG
    template <typename T>
    EventMessage(event_id eventId, T *data, bool handleable = true)
      : data(data), _eventId(eventId), event_type(typeid(T)), _handleable(handleable)
    {
    }
#else
    inline EventMessage(event_id eventId, void *data, bool handleable = true)
      : data(data), _eventId(eventId)
    {
      UNREFERENCED_PARAMETER(handleable);
    }
#endif

    template <typename T>
    T& getData()
    {
#ifdef _DEBUG
      if (typeid(T) != _event_type)
      {
        if (IsDebuggerPresent())
          __debugbreak();
        else
          throw std::exception("Tried to read event data as different type than stored");
      }
#endif
      return *reinterpret_cast<T *>(data);
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

    static event_id createEventId(const std::string& name);
  };

  class EventReciever abstract
  {
  public:
    virtual bool canHandle(const EventMessage& e) = 0;
    virtual void handle(EventMessage& e) = 0;
  };

  class EventDispatcher abstract : public EventReciever
  {
  public:
    virtual void addListener(EventReciever& reciever, event_id id) = 0;
    virtual void removeListener(EventReciever& reciever, event_id id) = 0;

    static std::unique_ptr<EventDispatcher> createDefaultDispatcher();
  };

  class Event
  {
  public:
    Event() {}
    
    template <typename T>
    bool raise(T *data);
  };
}
