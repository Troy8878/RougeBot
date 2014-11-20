/*********************************
 * IdleBehaviour.h
 * Jake Robsahm
 * Created 2014/11/19
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "AIBehaviour.h"

class IdleBehaviour : AIBehaviour
{
public:
  virtual void ApplyBehaviour(const WorldSnapshot &world) override; // Called on AI thread, do not access engine
  virtual void Prepare() override; // Called before AI thread is used
  virtual void InitializeTarget(Entity *target) override;
  virtual void InitilizeOwner(Entity *thisEntity) override;
  virtual AIResult GetResult() override;

private:

};

