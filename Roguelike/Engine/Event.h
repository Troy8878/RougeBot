/*********************************
 * Event.h
 * Connor Hilarides
 * Created 2014/06/28
 *********************************/

#pragma once

#include "Common.h"

#include "mruby.h"

// ----------------------------------------------------------------------------

typedef mrb_sym event_id;

// ----------------------------------------------------------------------------

namespace Events
{
  struct EventData;

// ----------------------------------------------------------------------------

  class EventMessage
  {
  public:
#ifdef _DEBUG
    EventMessage(event_id eventId, EventData *data, bool handleable = true)
      : _Data(data), _EventId(eventId), _handleable(handleable)
    {
    }
#else
    inline EventMessage(event_id eventId, EventData *data, bool handleable = true)
      : _Data(data), _EventId(eventId)
    {
      UNREFERENCED_PARAMETER(handleable);
    }
#endif

    IR_PROPERTY(EventData *, Data);
    IR_PROPERTY(event_id, EventId);

    template <typename T>
    T *GetData()
    {
      return static_cast<T *>(Data);
    }

    #pragma region Handled Property
#ifdef _DEBUG
    bool _handleable = true;
#endif
    bool _handled = false;

    __declspec(property(get = _GetHandled, put = _SetHandled)) bool Handled;
    inline void _SetHandled(bool handled) 
    { 
#ifdef _DEBUG
      if (handled && !_handleable)
        throw basic_exception("Tried to set that an event was handled on an event that cannot be handled");
#endif
      _handled = handled; 
    }
    inline bool _GetHandled() const { return _handled; }
    #pragma endregion
  };

// ----------------------------------------------------------------------------

  class EventReciever abstract
  {
  public:
    virtual bool CanHandle(const EventMessage& e) = 0;
    virtual void Handle(EventMessage& e) = 0;

  protected:
    inline ~EventReciever() {}
  };

  class EventDispatcher abstract : public EventReciever
  {
  public:
    virtual void AddListener(EventReciever *reciever) = 0;
    virtual void RemoveListener(EventReciever *reciever) = 0;

    static std::unique_ptr<EventDispatcher> CreateDefaultDispatcher();

  protected:
    inline ~EventDispatcher() {}
  };

// ----------------------------------------------------------------------------

  class Event
  {
  public:
    Event() {}

    static event_id CreateEventId(const std::string& name);
    static void Raise(EventMessage& e, EventReciever& reciever = *Event::GlobalDispatcher);

    static void CustomRaise(EventMessage& e, std::function<void(EventMessage&)> raise);

    static EventMessage& GetCurrentEvent();

    static EventDispatcher *GlobalDispatcher;
  };

// ----------------------------------------------------------------------------

#define DEF_EVENT_ID(id) static ::Events::EventId id{#id}

  class EventId
  {
    event_id _id;

  public:
    EventId(const std::string& str)
      : _id(Event::CreateEventId(str))
    {
    }

    inline operator event_id() { return _id; }
  };

// ----------------------------------------------------------------------------

}
