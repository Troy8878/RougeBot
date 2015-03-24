/*********************************
 * Autoplay.cpp
 * Connor Hilaides
 * Created 2015/03/23
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include <json/json.h>
#include "Autoplay.h"
#include <Game/PositionComponent.h>

void Autoplay::ApplyBehaviour(const WorldSnapshot& world, json::value params)
{
  auto left = world.GetTile(ox - 1, oy);
  auto right = world.GetTile(ox + 1, oy);
  auto up = world.GetTile(ox, oy - 1);
  auto down = world.GetTile(ox, oy + 1);

  if (left.actor != WorldSnapshot::Tile::Empty)
  {
    result.x = ox - 1;
    result.y = oy;
    result.action = AIResult::Attack;
  }
  else if (right.actor != WorldSnapshot::Tile::Empty)
  {
    result.x = ox + 1;
    result.y = oy;
    result.action = AIResult::Attack;
  }
  else if (up.actor != WorldSnapshot::Tile::Empty)
  {
    result.x = ox;
    result.y = oy - 1;
    result.action = AIResult::Attack;
  }
  else if (down.actor != WorldSnapshot::Tile::Empty)
  {
    result.x = ox;
    result.y = oy + 1;
    result.action = AIResult::Attack;
  }
  else
  {
    IdleBehaviour::ApplyBehaviour(world, params);
  }
}

void Autoplay::Prepare()
{
  IdleBehaviour::Prepare();
}

void Autoplay::InitilizeOwner(Entity* thisEntity)
{
  IdleBehaviour::InitilizeOwner(thisEntity);
}

AIResult Autoplay::GetResult()
{
  return result;
}
