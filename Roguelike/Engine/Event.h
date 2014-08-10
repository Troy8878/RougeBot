/*********************************
 * Event.h
 * Connor Hilarides
 * Created 2014/06/28
 *********************************/

#pragma once

#include "Common.h"

namespace Events
{
  class EventMessage
  {
    void *_data;
    bool _handled = false;
  #ifdef _DEBUG
    std::type_index _event_type;
  #endif

  public:
  #ifdef _DEBUG
    template <typename T>
    EventMessage(T *data)
      : data(data), event_type(typeid(T))
    {
    }
  #else
    inline EventMessage(void *data)
      : data(data)
    {
    }
  #endif

    template <typename T>
    inline T& getData()
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

    inline void handled(bool handled) { _handled = handled; }
    inline bool handled() const { return _handled; }
  };
  
  class EventHandler abstract
  {
    const bool _alwaysHandle;

  protected:
    EventHandler(bool alwaysHandle)
      : _alwaysHandle(alwaysHandle)
    {
    }

  public:
    virtual void operator()(EventMessage&) = 0;
    inline bool alwaysHandle() { return _alwaysHandle; }
  };

  class EventDispatcher abstract
  {

  };

  class Event
  {
  public:
    Event() {}
    
    template <typename T>
    bool raise(T *data);
  };
}
