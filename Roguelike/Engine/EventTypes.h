/*********************************
 * EventTypes.h
 * Connor Hilarides
 * Created 2014/08/12
 *********************************/

#pragma once

#include "RubyInterop.h"
#include "Common.h"

namespace Events
{
  class GameTime;

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
    RudimentaryEventWrapper() = default;

    RudimentaryEventWrapper(const T& data)
      : data(data)
    {
    }

    T data;
  };

  struct UpdateEvent
  {
    UpdateEvent(GameTime *gameTime)
      : gameTime(gameTime)
    {
    }

    GameTime *gameTime;
  };

}

