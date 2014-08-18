/*********************************
 * Level.h
 * Connor Hilarides
 * Created 2014/07/07
 *********************************/

#pragma once

#include "Common.h"
#include "EventHandlers.h"

// ----------------------------------------------------------------------------

class Game;

// ----------------------------------------------------------------------------

class Level : public Events::BasicEventDispatcher
{
public:
  Level();

  virtual void load() = 0;
  virtual void init() = 0;
  virtual void free() = 0;
  virtual void unload() = 0;

protected:
  Events::BasicClassEventReciever<Level> levelEvents;
  friend class Game;
};

// ----------------------------------------------------------------------------

