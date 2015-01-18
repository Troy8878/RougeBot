/*********************************
 * AISystem.cpp
 * Connor Hilarides
 * Created 2015/01/16
 * Copyright � 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "AISystem.h"

// ----------------------------------------------------------------------------

static std::unordered_map<std::string, AIFactory *> factories;

void AIFactory::Register(std::string name, AIFactory *factory)
{
  factories[name] = factory;
}

// ----------------------------------------------------------------------------

AIDecisionRef AIDecision::New(AIFactory &factory)
{
  return AIDecisionRef(new AIDecision(factory));
}

// ----------------------------------------------------------------------------

option<AIResult> AIDecision::GetResult()
{
  if (hasResult)
  {
    return result;
  }

  return {};
}

// ----------------------------------------------------------------------------

AIDecision::AIDecision(AIFactory &factory)
  : behaviour(factory.Create()), hasResult(false)
{
}

// ----------------------------------------------------------------------------

