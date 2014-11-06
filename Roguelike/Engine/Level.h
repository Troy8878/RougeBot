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
  static Level *CreateLevel(const std::string &def);
  static void DestroyLevel(Level *level);

  IR_PROPERTY(std::string, Name);
  IR_PROPERTY(Entity *, RootEntity);

protected:
  Level();
  ~Level();

  Events::BasicEventDispatcher levelEvents;

  friend class Game;
  friend class LevelDef;
};

// ----------------------------------------------------------------------------