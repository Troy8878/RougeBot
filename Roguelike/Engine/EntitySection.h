/*********************************
 * Level.h
 * Connor Hilarides
 * Created 2014/08/19
 *********************************/

#pragma once

#include "Common.h"
#include "EventHandlers.h"

// ----------------------------------------------------------------------------

class EntitySection
{
public:
  EntitySection();

  void AddEntity(Entity *entity);
  void RemoveEntity(Entity *entity);
};

// ----------------------------------------------------------------------------