/*********************************
 * AIBehaviour.h
 * Jake Robsahm
 * Created 2014/10/29
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once
#include "mruby.h"

class WorldSnapshot;
class Entity;

struct AIResult
{
  enum ActionType
  {
    Nil,
    Move,
    Attack
  } action;

  mrb_int x, y;
};

__interface AIBehaviour
{
  void ApplyBehaviour(const WorldSnapshot &world); // Called on AI thread, do not access engine
  void Prepare(); // Called before AI thread is used
  void InitializeTarget(Entity *target);
  void InitilizeOwner(Entity *thisEntity);
  AIResult GetResult();
};
