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
  world;
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