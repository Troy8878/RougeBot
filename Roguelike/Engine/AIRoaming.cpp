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


// ----------------------------------------------------------------------------

void AIRoaming::ApplyBehaviour(const WorldSnapshot &world)
{
  float distance = sqrtf(static_cast<float>((ox - tx) * (ox - tx) + (oy - ty) * (oy - ty)));

  if (distance < 1.1)
  {
    result.action = AIResult::Attack;
    result.x = tx;
    result.y = ty;
  }
  else if (distance <20)
  {
    MoveTowards(world);
  }
  else
  {
    result.action = AIResult::Nil;
  }
}

// ----------------------------------------------------------------------------

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
  mrb_int ax = std::abs(dx);
  mrb_int ay = std::abs(dy);

  //bool canMoveUp = world.CanMove(ox, oy, 0, 1) == WorldSnapshot::NotBlocked;
  //bool canMoveDown = world.CanMove(ox, oy, 0, -1) == WorldSnapshot::NotBlocked;
  //bool canMoveRight = world.CanMove(ox, oy, 1, 0) == WorldSnapshot::NotBlocked;
  bool canMoveLeft = world.CanMove(ox, oy, -1, 0) == WorldSnapshot::NotBlocked;

  // Check if we need to move on the x axis before the y axis.
  if (ax > ay)
  {
    // Check if we need to move left.
    if (dx > 0)
    {
      if ( canMoveLeft )
      {
        result.action = AIResult::Move;
        result.x = ox - 1;
        result.y = oy;
        return;
      }
      // Otherwise, check to see if we want to move up/down.
      else if (ay)
      {
        // Check if we want to move down.
        if (dy > 0)
        {
          
        }
      }

    }
  }
}


