/*********************************
 * AIRoaming.h
 * Jake Robsahm
 * Created 2014/11/05
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "AIBehaviour.h"

class AIRoaming final : public AIBehaviour
{
public:
  void ApplyBehaviour(const WorldSnapshot &world) override;
  void Prepare() override;
  void InitializeTarget(Entity *target) override;
  void InitilizeOwner(Entity *thisEntity) override;
  AIResult GetResult() override;

private:
  void MoveTowards(const WorldSnapshot &world);
  

  Entity *owner;
  Entity *target;

  mrb_int ox, oy;
  mrb_int tx, ty;

  AIResult result;

};
