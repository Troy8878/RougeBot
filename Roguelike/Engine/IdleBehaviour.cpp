/*********************************
 * IdleBehaviour.cpp
 * Jake Robsahm
 * Created 2014/11/19
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "IdleBehaviour.h"
#include "RubyWrappers.h"
#include "WorldSnapshot.h"


void IdleBehaviour::ApplyBehaviour(const WorldSnapshot& world)
{
  // Randomly generate a number from 1 to 4 to determine direction.
  std::random_device rng;
  int direction = std::uniform_int_distribution<>(1, 4)(rng);

  // Store whether we can move in each direction ahead of time.
  bool canMoveUp = world.CanMove(ox, oy, 0, 1) == WorldSnapshot::NotBlocked;
  bool canMoveDown = world.CanMove(ox, oy, 0, -1) == WorldSnapshot::NotBlocked;
  bool canMoveRight = world.CanMove(ox, oy, 1, 0) == WorldSnapshot::NotBlocked;
  bool canMoveLeft = world.CanMove(ox, oy, -1, 0) == WorldSnapshot::NotBlocked;

  // Move in the appropriate direction based on the result.
  switch (direction)
  {
    case 1:
      if (canMoveUp)
      {
        MoveUp();
        break;
      }
    case 2:
      if (canMoveDown)
      {
        MoveDown();
        break;
      }
    case 3:
      if (canMoveLeft)
      {
        MoveLeft();
        break;
      }
    case 4:
      if (canMoveRight)
      {
        MoveRight();
        break;
      }
    default:
      result.action = AIResult::Nil;
  }
}

void IdleBehaviour::Prepare()
{
  // Get the current position
  static mrb_sym get_position = mrb_intern_lit(*mrb_inst, "position");

  // Find the owner's position component and set the locations appropriately.
  auto ownerPosComp = owner->GetComponent("PositionComponent")->GetRubyWrapper();
  auto &ownerPos = ruby::get_ruby_vector(mrb_funcall_argv(*mrb_inst, ownerPosComp, get_position, 0, nullptr));
  ox = static_cast<mrb_int>(ownerPos.x);
  oy = static_cast<mrb_int>(ownerPos.y);
}

void IdleBehaviour::InitializeTarget(Entity* )
{
}

void IdleBehaviour::InitilizeOwner(Entity *thisEntity)
{
  owner = thisEntity;
}

AIResult IdleBehaviour::GetResult()
{
  return result;
}
