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
  // Find the start and end tiles.
  auto *startTile = &world.GetTile(ox, oy);
  auto *targetTile = &world.GetTile(tx, ty);

  // Assign the tiles to their nodes.
  startNode.tile = startTile;
  targetNode.tile = targetTile;

  // Calculate the cost of the starting node.
  startNode.prevCost = 0;
  startNode.costToGoal = static_cast<float>(std::abs(ox - tx) + std::abs(oy - ty));
  startNode.cost = startNode.prevCost + startNode.costToGoal;

  // Add the starting node to the openNodes list.
  openNodes.push_back(startNode);

  // While there are still open nodes
  while (!openNodes.empty())
  {
    // Initialize some stuff.
    Node *pnext = nullptr;
    // Basically this is the highest possible float (roughly).
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
    // Save the cheapest node.
    Node next = *pnext;

    // Remove the cheapest node from the open list - it's been checked.
    openNodes.erase(openNodes.begin() + (pnext - &openNodes[0]));

    // Temporary garbage bullshit.
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
    std::vector<Node> successorNodes;
    for (auto offset : offsets)
    {
      // This takes way too goddamn long.
      const WorldSnapshot::Tile *tile;
      tile = &world.GetTile(next.tile->x + offset.x, next.tile->y + offset.y);
      Node temp(new Node(next), tile);
      temp.prevCost = next.prevCost + 1;
      temp.costToGoal = static_cast<float>(std::abs(temp.tile->x - tx) + std::abs(temp.tile->y - ty));
      temp.cost = temp.prevCost + temp.costToGoal;
      successorNodes.push_back(temp);
    }

    // Find the successors we want to check after this one.
    for (auto node : successorNodes)
    {
      // If this is the destination, we're done. Go us! Add it to the open list.
      if (node.tile == targetNode.tile)
      {
        openNodes.push_back(node);
        break;
      }
      
      // Temporary boolean to track if we wanna add this to the open list.
      bool addToList = true;

      // Find if there's already a node in the open list that's cheaper.
      for (auto open : openNodes)
      {
        if (node.tile == open.tile && open.cost < node.cost)
        {
          addToList = false;
          break;
        }
      }

      // Find if there's a node in the closed list that's cheaper.
      for (auto closed : closedNodes)
      {
        if (node.tile == closed.tile && closed.cost < node.cost)
        {
          addToList = false;
          break;
        }
      }
      
      // If it's good to go, we add it to the openNodes list.
      // Basically, it gets checked on the next pass of the while loop.
      if (addToList)
        openNodes.push_back(node);
    }

    // Finally, we can add the chosen node from before (the one we removed from the open list)
    // to the closed list.
    closedNodes.push_back(next);
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
  this->target = target;
}

void AStarPathfinding::InitilizeOwner(Entity* thisEntity)
{
  owner = thisEntity;
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