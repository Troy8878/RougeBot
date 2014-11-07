/*********************************
 * AIBehaviour.h
 * Jake Robsahm
 * Created 2014/10/29
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "AIBehaviour.h"

class AStarPathfinding : public AIBehaviour
{

public:
  void ApplyBehaviour(const WorldSnapshot& world) override;

  void SetTarget(Entity *targetEntity);
  
  
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
