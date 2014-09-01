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

  IR_PROPERTY(std::string, Name);
  IR_PROPERTY(Entity *, RootEntity);

  virtual void Load();
  virtual void Init();
  virtual void Free();
  virtual void Unload();

protected:
  Events::BasicClassEventReciever<Level> levelEvents;

  friend class Game;
  friend class LevelDef;
};

// ----------------------------------------------------------------------------

