/*********************************
 * AIDerp.h
 * Connor Hilarides
 * Created 2015/02/03
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "AIBehaviour.h"

class AIDerp final : public AIBehaviour
{
public:
  virtual void ApplyBehaviour(const WorldSnapshot &world, json::value params) override;
  virtual void Prepare() override; // Called before AI thread is used
  virtual void InitializeTarget(Entity *target) override;
  virtual void InitilizeOwner(Entity *thisEntity) override;
  virtual AIResult GetResult() override;

private:
  AIResult result;
};

