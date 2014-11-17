/*********************************
 * AIBehaviour.h
 * Jake Robsahm
 * Created 2014/10/29
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "AIBehaviour.h"

class AStarPathfinding final : public AIBehaviour
{
public:

  void ApplyBehaviour(const WorldSnapshot& world) override;

  void Prepare() override;
  void InitializeTarget(Entity* target) override;
  void InitilizeOwner(Entity* thisEntity) override;
  AIResult GetResult() override;
  
private:
  struct Route
  {

  };

  const WorldSnapshot *world;
  Entity *thisEntity;
  Entity *target;
  Route *currentRoute;

  void FindRoute();
  void MoveAlongRoute();
};
