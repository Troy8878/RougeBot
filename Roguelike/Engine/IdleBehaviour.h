/*********************************
 * IdleBehaviour.h
 * Claire Robsahm
 * Created 2014/11/19
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "AIBehaviour.h"

class IdleBehaviour : public AIBehaviour
{
public:
  virtual void ApplyBehaviour(const WorldSnapshot &world, json::value params) override; // Called on AI thread, do not access engine
  virtual void Prepare() override; // Called before AI thread is used
  
  virtual void InitilizeOwner(Entity *thisEntity) override;
  virtual void InitializeTarget(Entity *) override {}
  virtual AIResult GetResult() override;
};

