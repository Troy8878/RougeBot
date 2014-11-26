/*********************************
 * AIBehaviour.h
 * Jake Robsahm
 * Created 2014/10/29
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include <vector>
#include "AIBehaviour.h"
#include "WorldSnapshot.h"

class AStarPathfinding final : public AIBehaviour
{
public:
  void ApplyBehaviour(const WorldSnapshot& world) override;

  void Prepare() override;
  void InitializeTarget(Entity* target) override;
  void InitilizeOwner(Entity* thisEntity) override;
  AIResult GetResult() override;
  
private:
  struct Node
  {
    Node() = default;
    explicit Node(const WorldSnapshot::Tile *tile);
    explicit Node(Node *previous, const WorldSnapshot::Tile *tile);

    Node *previous;
    const WorldSnapshot::Tile *tile;
    bool evaluated;

    float cost;
    float prevCost;
    float costToGoal;
  };

  const WorldSnapshot *world;
  Entity *thisEntity;
  Entity *target;
  Node startNode;
  Node targetNode;

  // The nodes that we still need to check.
  std::vector<Node> openNodes;
  // the nodes that we will be following.
  std::vector<Node> closedNodes;

  void MoveAlongRoute();
};
