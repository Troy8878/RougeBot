/*********************************
 * AIRoaming.h
 * Claire Robsahm
 * Created 2014/11/05
 * Copyright � 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "AIBehaviour.h"
#include "IdleBehaviour.h"

class AIRoaming final : public IdleBehaviour
{
public:
  void ApplyBehaviour(const WorldSnapshot &world, json::value params) override;
  void Prepare() override;
  void InitializeTarget(Entity *target) override;
  void InitilizeOwner(Entity *thisEntity) override;
  AIResult GetResult() override;

private:
  void MoveTowards(const WorldSnapshot &world);

  Entity *target;
};
