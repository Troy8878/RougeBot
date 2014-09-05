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
  ~Level();

  IR_PROPERTY(std::string, Name);
  IR_PROPERTY(Entity *, RootEntity);

  virtual void Init();
  virtual void Free();

protected:
  Events::BasicClassEventReciever<Level> levelEvents;

  friend class Game;
  friend class LevelDef;
};

// ----------------------------------------------------------------------------

