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

  struct EventData abstract
  {
    virtual ruby::ruby_value getRubyWrapper() = 0;
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

