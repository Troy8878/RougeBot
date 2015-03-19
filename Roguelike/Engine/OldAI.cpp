/*********************************
 * OldAI.cpp
 * Connor Hilarides
 * Created 2015/03/19
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "OldAI.h"
#include "WorldSnapshot.h"

void OldAI::ApplyBehaviour(const WorldSnapshot &world, json::value)
{
}

void OldAI::Prepare()
{
}

void OldAI::InitilizeOwner(Entity *thisEntity)
{

}

AIResult OldAI::GetResult()
{
  return result;
}
