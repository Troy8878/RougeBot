/*********************************
 * AIRoaming.h
 * Claire Robsahm
 * Created 2014/11/05
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

// This is a test AI behaviour! It is written like shit, and works even shittier. Don't use it in production!


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
