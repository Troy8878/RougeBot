/*********************************
 * AIBehaviour.cpp
 * Jake Robsahm
 * Created 2014/11/19
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "AIBehaviour.h"
#include "RubyWrappers.h"
#include "WorldSnapshot.h"

// ----------------------------------------------------------------------------

void AIBehaviour::MoveLeft()
{
  result.action = AIResult::Move;
  result.x = ox - 1;
  result.y = oy;
}

// ----------------------------------------------------------------------------

void AIBehaviour::MoveRight()
{
  result.action = AIResult::Move;
  result.x = ox + 1;
  result.y = oy;
}

void AIBehaviour::MoveUp()
{
  result.action = AIResult::Move;
  result.x = ox;
  result.y = oy + 1;
}

// ----------------------------------------------------------------------------

void AIBehaviour::MoveDown()
{
  result.action = AIResult::Move;
  result.x = ox;
  result.y = oy - 1;
}
