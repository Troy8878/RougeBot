/*********************************
 * AIAngstMage.cpp
 * Claire Robsahm
 * Created 2015/4/09
 * Copyright ｩ 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

// This is a test AI behaviour! It is written like shit, and works even shittier. Don't use it in production!

#include "Common.h"
#include "AIAngstMage.h"
#include "RubyWrappers.h"
#include "WorldSnapshot.h"
#include <json/jquery.h>

// ----------------------------------------------------------------------------

void AIAngstMage::ApplyBehaviour(const WorldSnapshot &world, json::value params)
{
  double aggro_range = 0;
  double attack_range = 1.1;
  int heal_value = 10;

  query(params).at("aggro_range").is_number().assign(aggro_range);
  query(params).at("attack_range").is_number().assign(attack_range);
  query(params).at("heal_value").is_number().assign(heal_value);

  float distance = sqrtf(static_cast<float>((ox - tx) * (ox - tx) + (oy - ty) * (oy - ty)));

  std::cout << "1" << std::endl;
  if (distance < attack_range)
  {
    int res = std::uniform_int_distribution<int>(0, 2)(std::random_device());
    
    std::cout << "2 => " << res << std::endl;
    if (res == 0)
    {
      result.action = AIResult::Custom;
      
      auto data = json::value::object();
      data["action"] = json::value::string("heal");
      data["value"] = json::value::number(static_cast<double>(heal_value));
      data["x"] = json::value::number(static_cast<double>(tx));
      data["y"] = json::value::number(static_cast<double>(ty));
      result.custom = data.serialize();
    }
    else if (res == 1)
    {
      IdleBehaviour::ApplyBehaviour(world, params);
    }
    else
    {
      result.action = AIResult::Attack;
      result.x = tx;
      result.y = ty;
    }
  }
  else if (distance < aggro_range)
  {
    MoveTowards(world);
  }
  else
  {
    IdleBehaviour::ApplyBehaviour(world, params);
  }
}

// ----------------------------------------------------------------------------

void AIAngstMage::Prepare()
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

void AIAngstMage::InitializeTarget(Entity *target)
{
  this->target = target;
}

// ----------------------------------------------------------------------------

void AIAngstMage::InitilizeOwner(Entity *thisEntity)
{
  owner = thisEntity;
}

// ----------------------------------------------------------------------------

AIResult AIAngstMage::GetResult()
{
  return result;
}

// ----------------------------------------------------------------------------
// Private functions

void AIAngstMage::MoveTowards(const WorldSnapshot &world)
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
