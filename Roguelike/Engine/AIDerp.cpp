/*********************************
 * AIDerp.cpp
 * Connor Hilarides
 * Created 2015/02/03
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "AIDerp.h"
#include "WorldSnapshot.h"

void AIDerp::ApplyBehaviour(const WorldSnapshot &, json::value)
{
  result = AIResult
  {
    AIResult::Custom,
    0, 0,
    R"( {"action":"message","text":"derp"} )"
  };
}

void AIDerp::Prepare()
{
}

void AIDerp::InitializeTarget(Entity *)
{
}

void AIDerp::InitilizeOwner(Entity *)
{
}

AIResult AIDerp::GetResult()
{
  return result;
}

