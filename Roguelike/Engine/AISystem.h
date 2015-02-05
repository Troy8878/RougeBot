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
  virtual AIBehaviour *Create() = 0;

  virtual ~AIFactory()
  {
  }

  static void Register(const std::string &name, AIFactory *factory);
  static AIFactory *GetFactory(const std::string &name);

  static void RegisterDefaultFactories();
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
  std::atomic<bool> quit = false;

  void RunThread();
  AIDecisionRef GetDecision();
};

// ----------------------------------------------------------------------------

template <typename T>
class DefaultAIFactory final : public AIFactory
{
public:
  virtual AIBehaviour *Create() override
  {
    return new T;
  }
};

// ----------------------------------------------------------------------------


