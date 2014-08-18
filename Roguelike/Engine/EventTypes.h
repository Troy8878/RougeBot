/*********************************
 * EventTypes.h
 * Connor Hilarides
 * Created 2014/08/12
 *********************************/

#pragma once

#include "RubyInterop.h"
#include "Common.h"

class GameTime;

namespace Events
{

  struct EventData
  {
    virtual ruby::ruby_value getRubyWrapper() 
    { 
      return ruby::ruby_value{}; 
    };
  };

  template <typename T>
  struct RudimentaryEventWrapper : public EventData
  {
    T data;

    RudimentaryEventWrapper() = default;
    RudimentaryEventWrapper(const T& data)
      : data(data)
    {
    }

    NO_ASSIGNMENT_OPERATOR(RudimentaryEventWrapper<T>);
  };

  struct UpdateEvent : public EventData
  {
    GameTime& gameTime;

    UpdateEvent(GameTime& gameTime)
      : gameTime(gameTime)
    {
    }

    NO_ASSIGNMENT_OPERATOR(UpdateEvent);
  };

}

