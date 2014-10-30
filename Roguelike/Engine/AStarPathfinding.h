/*********************************
 * AIBehaviour.h
 * Jake Robsahm
 * Created 2014/10/29
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
