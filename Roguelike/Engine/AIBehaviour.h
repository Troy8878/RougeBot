/*********************************
 * AIBehaviour.h
 * Claire Robsahm
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

class AIBehaviour
{
public:
  virtual ~AIBehaviour() {}

  virtual void ApplyBehaviour(const WorldSnapshot &world) = 0; // Called on AI thread, do not access engine
  virtual void Prepare() = 0; // Called before AI thread is used
  virtual void InitializeTarget(Entity *target) = 0;
  virtual void InitilizeOwner(Entity *thisEntity) = 0;
  virtual AIResult GetResult() = 0;

protected:
  virtual void MoveLeft();
  virtual void MoveRight();
  virtual void MoveUp();
  virtual void MoveDown();

  Entity *owner;

  mrb_int ox, oy;
  mrb_int tx, ty;

  AIResult result;
};

