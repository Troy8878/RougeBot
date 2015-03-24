/*********************************
 * Autoplay.h
 * Connor Hilaides
 * Created 2015/03/23
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "AIBehaviour.h"
#include "IdleBehaviour.h"

class Autoplay final : public IdleBehaviour
{
public:
  void ApplyBehaviour(const WorldSnapshot &world, json::value params) override;
  void Prepare() override;
  void InitilizeOwner(Entity *thisEntity) override;
  AIResult GetResult() override;
};

