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

  virtual void Load() = 0;
  virtual void Init() = 0;
  virtual void Free() = 0;
  virtual void Unload() = 0;

protected:
  Events::BasicClassEventReciever<Level> levelEvents;
  friend class Game;
};

// ----------------------------------------------------------------------------

template <typename T>
class LevelRegistration
{
  LevelRegistration(const std::string& name);
};

// ----------------------------------------------------------------------------

