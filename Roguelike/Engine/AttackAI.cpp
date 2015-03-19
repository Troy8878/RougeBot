/*********************************
 * AttackAI.cpp
 * Connor Hilarides
 * Created 2015/03/19
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "AttackAI.h"
#include "WorldSnapshot.h"
#include <json/jquery.h>

void AttackAI::ApplyBehaviour(const WorldSnapshot& world, json::value params)
{
  range = 1.1;
  query(params).at("range").is_number().assign(range);
}

void AttackAI::Prepare()
{
}

void AttackAI::InitilizeOwner(Entity* thisEntity)
{
}

AIResult AttackAI::GetResult()
{
}
