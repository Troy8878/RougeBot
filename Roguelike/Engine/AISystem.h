/*********************************
 * AISystem.h
 * Connor Hilarides
 * Created 2015/01/16
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Common.h"
#include "AIBehaviour.h"

// ----------------------------------------------------------------------------

class AIFactory
{
public:
  virtual AIBehaviour *Create() = 0;

  virtual ~AIFactory()
  {
  }

  static void Register(std::string name, AIFactory *factory);
};

// ----------------------------------------------------------------------------

class AIDecision;
typedef std::shared_ptr<AIDecision> AIDecisionRef;

class AIDecision
{
public:
  static AIDecisionRef New(AIFactory &factory);
  option<AIResult> GetResult();

  NO_COPY_CONSTRUCTOR(AIDecision);
  NO_ASSIGNMENT_OPERATOR(AIDecision);

private:
  explicit AIDecision(AIFactory &factory);

  AIBehaviour *behaviour;
  AIResult result;
  std::atomic<bool> hasResult;
};

// ----------------------------------------------------------------------------

class AISystem
{
public:
private:
  std::list<AIDecisionRef> pendingDecisions;
};

// ----------------------------------------------------------------------------

