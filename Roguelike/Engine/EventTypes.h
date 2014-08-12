/*********************************
 * EventTypes.h
 * Connor Hilarides
 * Created 2014/08/12
 *********************************/

#pragma once

#include "Common.h"

namespace Events
{
  class GameTime;

  struct EventData abstract
  {

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

