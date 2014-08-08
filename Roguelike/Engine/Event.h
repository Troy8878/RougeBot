/*********************************
 * Event.h
 * Connor Hilarides
 * Created 2014/06/28
 *********************************/

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
      extern "C" int __stdcall IsDebuggerPresent(void);

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

  class BasicFPEventHandler : public EventHandler
  {
    typedef void (*function_type)(EventMessage&);
    function_type func;

  public:
    BasicFPEventHandler(function_type func, bool alwaysHandle)
      : EventHandler(alwaysHandle), func(func)
    {
    }

    virtual void operator()(EventMessage& event) override
    {
      func(event);
    }
  };

  template <typename Class>
  class MemberFunctionEventHandler : public EventHandler
  {
    typedef void (Class::*function_type)(EventMessage&);
    function_type func;
    Class *object;

  public:
    MemberFunctionEventHandler(function_type func, Class *object, bool alwaysHandle)
      : EventHandler(alwaysHandle), func(func), object(object)
    {
    }

    virtual void operator()(EventMessage& event) override
    {
      (object ->* func)(event);
    }
  };

  class StdFuncEventHandler : public EventHandler
  {
  };

  class Event
  {
    std::vector<EventHandler *> _definitehandlers;
    std::vector<EventHandler *> _handlers;

  public:
    Event() {}

    void addHandler(EventHandler *handler)
    {
      if (handler->alwaysHandle())
        _definitehandlers.push_back(handler);
      else
        _handlers.push_back(handler);
    }

    template <typename T>
    bool raise(T *data)
    {
      EventMessage message{data};

      for (auto *handler : _handlers)
      {
        (*handler)(message);
      }
      
      if (message.handled())
        return true;

      for (auto *handler : _handlers)
      {
        (*handler)(message);
      
        if (message.handled())
          return true;
      }

      return false;
    }
  };
}
