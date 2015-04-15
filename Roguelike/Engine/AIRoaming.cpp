/*********************************
 * AIRoaming.cpp
 * Claire Robsahm
 * Created 2014/11/09
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

// This is a test AI behaviour! It is written like shit, and works even shittier. Don't use it in production!

#include "Common.h"
#include "AIRoaming.h"
#include "RubyWrappers.h"
#include "WorldSnapshot.h"
#include <json/jquery.h>

// Conner, I'm sorry...
static bool beam = false;

// ----------------------------------------------------------------------------

void AIRoaming::ApplyBehaviour(const WorldSnapshot &world, json::value params)
{
  double aggro_range = 3.5;
  double attack_range = 1.1;
  bool attack_ticktock = false;

  query(params).at("aggro_range").is_number().assign(aggro_range);
  query(params).at("attack_range").is_number().assign(attack_range);
  query(params).at("attack_ticktock").is_bool().assign(attack_ticktock);

  float distance = sqrtf(static_cast<float>((ox - tx) * (ox - tx) + (oy - ty) * (oy - ty)));

  if (distance < attack_range)
  {
    if (beam || !attack_ticktock)
    {
      result.action = AIResult::Attack;
      result.x = tx;
      result.y = ty;
      beam = false;
    }
    else
    {
      result.action = AIResult::Custom;
      result.custom = R"( {"action":"colorize", "color": "Red", "turns": 1} )";
      beam = true;
    }
  }
  else if (distance < aggro_range)
  {
    MoveTowards(world);
    beam = false;
  }
  else
  {
    IdleBehaviour::ApplyBehaviour(world, params);
  }
}

// ----------------------------------------------------------------------------

void AIRoaming::Prepare()
{
  IdleBehaviour::Prepare();

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

// ----------------------------------------------------------------------------

void AIRoaming::InitializeTarget(Entity *target)
{
  this->target = target;
}

// ----------------------------------------------------------------------------

void AIRoaming::InitilizeOwner(Entity *thisEntity)
{
  owner = thisEntity;
}

// ----------------------------------------------------------------------------

AIResult AIRoaming::GetResult()
{
  return result;
}

// ----------------------------------------------------------------------------
// Private functions

void AIRoaming::MoveTowards(const WorldSnapshot &world)
{
  // Difference between owner and target.
  mrb_int dx = ox - tx;
  mrb_int dy = oy - ty;
  // Absolute values between differences
  // mrb_int ax = std::abs(dx);
  // mrb_int ay = std::abs(dy);

  // Store whether we can move in each direction ahead of time.
  bool canMoveUp = world.CanMove(ox, oy, 0, 1) == WorldSnapshot::NotBlocked;
  bool canMoveDown = world.CanMove(ox, oy, 0, -1) == WorldSnapshot::NotBlocked;
  bool canMoveRight = world.CanMove(ox, oy, 1, 0) == WorldSnapshot::NotBlocked;
  bool canMoveLeft = world.CanMove(ox, oy, -1, 0) == WorldSnapshot::NotBlocked;

  // Check if we are to the right of the target.
  if (dx > 0 && canMoveLeft)
  {
    MoveLeft();
    return;
  }

  // Check if we are to the left of the target.
  if (dx < 0 && canMoveRight)
  {
    MoveRight();
    return;
  }

  // Check if we are below the target.
  if (dy > 0 && canMoveDown)
  {
    MoveDown();
    return;
  }

  // Check if we are above the target.
  if (dy < 0 && canMoveUp)
  {
    MoveUp();
    return;
  }
}
