/*********************************
 * AIBehaviour.cpp
 * Jake Robsahm
 * Created 2014/11/24
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "AStarPathfinding.h"
#include "RubyWrappers.h"

void AStarPathfinding::ApplyBehaviour(const WorldSnapshot& world)
{
  auto *startTile = &world.GetTile(ox, oy);
  auto *targetTile = &world.GetTile(tx, ty);

  startNode.tile = startTile;
  targetNode.tile = targetTile;

  startNode.prevCost = 0;
  startNode.costToGoal = (float)(std::abs(ox - tx) + std::abs(oy - ty));
  startNode.cost = startNode.prevCost + startNode.costToGoal;

  openNodes.push_back(startNode);

  while (!openNodes.empty())
  {
    Node *pnext;
    float currCost = powf(10, 300);
    // Find the lowest cost node in the openNode vector.
    for (auto &node : openNodes)
    {
      if (node.cost < currCost)
      {
        pnext = &node;
        currCost = pnext->cost;
      }
    }
    Node next = *pnext;
    openNodes.erase(openNodes.begin() + (pnext - &openNodes[0]));

    struct
    {
      int x;
      int y;
    } offsets[8] =
    {
      {-1,  0}, // left
      { 1,  0}, // right
      { 0,  1}, // up
      { 0, -1}  // down
    };

    // Set the successors.
    std::vector<Node> successorList;
    for (auto offset : offsets)
    {
      const WorldSnapshot::Tile *tile;
      tile = &world.GetTile(next.tile->x + offset.x, next.tile->y + offset.y);
      Node temp(new Node(next), tile);
      temp.prevCost = next.prevCost + 1;
      temp.costToGoal = (float)(std::abs(temp.tile->x - tx) + std::abs(temp.tile->y - ty));
      temp.cost = temp.prevCost + temp.costToGoal;
      successorList.push_back(temp);
    }

    // Find the successor we want to follow.
    for (auto node : successorList)
    {
      if (node.tile == targetNode.tile)
      {
        break;
      }

    }

  }




}

void AStarPathfinding::Prepare()
{
   // Get the current position
  static mrb_sym get_position = mrb_intern_lit(*mrb_inst, "position");

  // Find the owner's position component and set the locations appropriately.
  auto ownerPosComp = owner->GetComponent("PositionComponent")->GetRubyWrapper();
  auto &ownerPos = ruby::get_ruby_vector(mrb_funcall_argv(*mrb_inst, ownerPosComp, get_position, 0, nullptr));
  ox = static_cast<mrb_int>(ownerPos.x);
  oy = static_cast<mrb_int>(ownerPos.y);

  // Find the target's position component and set the location sappropriately.
  auto targetPosComp = target->GetComponent("PositionComponent")->GetRubyWrapper();
  auto &targetPos = ruby::get_ruby_vector(mrb_funcall_argv(*mrb_inst, targetPosComp, get_position, 0, nullptr));
  tx = static_cast<mrb_int>(targetPos.x);
  ty = static_cast<mrb_int>(targetPos.y);
}

void AStarPathfinding::InitializeTarget(Entity* target)
{
  target;
}

void AStarPathfinding::InitilizeOwner(Entity* thisEntity)
{
  thisEntity;
}

AIResult AStarPathfinding::GetResult()
{
  return result;
}

AStarPathfinding::Node::Node(const WorldSnapshot::Tile *tile)
  : previous(nullptr), tile(tile)
{
  evaluated = false;
}

AStarPathfinding::Node::Node(Node* previous, const WorldSnapshot::Tile *tile)
  : previous(previous), tile(tile)
{
  evaluated = false;
}

void AStarPathfinding::FindRoute()
{
}

void AStarPathfinding::MoveAlongRoute()
{
}