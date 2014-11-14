/*********************************
 * AIRoaming.cpp
 * Jake Robsahm
 * Created 2014/11/09
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "AIRoaming.h"
#include "RubyWrappers.h"
#include "WorldSnapshot.h"

void AIRoaming::ApplyBehaviour(const WorldSnapshot &world)
{
  world;
  result.action = AIResult::Nil;
}

void AIRoaming::Prepare()
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

void AIRoaming::InitializeTarget(Entity *target)
{
  this->target = target;
}

void AIRoaming::InitilizeOwner(Entity *thisEntity)
{
  owner = thisEntity;
}

AIResult AIRoaming::GetResult()
{
  return result;
}

