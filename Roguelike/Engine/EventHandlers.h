/*********************************
 * EventHandlers.h
 * Connor Hilarides
 * Created 2014/08/09
 *********************************/

#pragma once

#include "Common.h"
#include "Event.h"

namespace Events
{

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
    typedef std::function<void(EventMessage&)> function_type;
    function_type func;

  public:
    StdFuncEventHandler(function_type func, bool alwaysHandle)
      : EventHandler(alwaysHandle), func(func)
    {
    }
  };

}
