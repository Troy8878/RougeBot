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
  // stub impl
  world;
  result.action = AIResult::Nil;
}

void AIRoaming::Prepare()
{
  static mrb_sym get_position = mrb_intern_lit(*mrb_inst, "position");

  auto ownerPosComp = owner->GetComponent("PositionComponent")->GetRubyWrapper();
  auto &ownerPos = ruby::get_ruby_vector(mrb_funcall_argv(*mrb_inst, ownerPosComp, get_position, 0, nullptr));
  ox = static_cast<mrb_int>(ownerPos.x);
  oy = static_cast<mrb_int>(ownerPos.y);
}

void AIRoaming::InitializeTarget(Entity *target)
{
  target;
}

void AIRoaming::InitilizeOwner(Entity *thisEntity)
{
  thisEntity;
}

AIResult AIRoaming::GetResult()
{
  return result;
}