/*********************************
 * AISystem.h
 * Connor Hilarides
 * Created 2015/01/16
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Common.h"
#include "AIBehaviour.h"
#include "WorldSnapshot.h"

// ----------------------------------------------------------------------------

class AIFactory
{
public:
  virtual AIBehaviour *Create(json::value params) = 0;

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
  static AIDecisionRef New(AIFactory &factory, json::value params);
  option<AIResult> GetResult();

  NO_COPY_CONSTRUCTOR(AIDecision);
  NO_ASSIGNMENT_OPERATOR(AIDecision);

  void Init(Entity *owner, Entity *target);
  void Run(const WorldSnapshot &snap, json::value params);

private:
  explicit AIDecision(AIFactory &factory, json::value params);

  json::value params;
  AIBehaviour *behavior;
  AIResult result;
  std::atomic<bool> hasResult;

  friend class AISystem;
};

// ----------------------------------------------------------------------------

class AISystem final
{
public:
  AISystem(size_t threadCount);
  ~AISystem();

  AIDecisionRef QueueBehavior(AIFactory &factory, json::value params,
                              Entity *owner, Entity *target);
  void UpdateSnapshot();

  NO_COPY_CONSTRUCTOR(AISystem);
  NO_ASSIGNMENT_OPERATOR(AISystem);

private:
  rwlock<WorldSnapshot> snapshot;
  object_lock<std::queue<AIDecisionRef>> decisionQueue;
  std::vector<std::thread> decisionThreads;
  std::atomic<bool> quit = true;

  void RunThread();
  AIDecisionRef GetDecision();
};

// ----------------------------------------------------------------------------

