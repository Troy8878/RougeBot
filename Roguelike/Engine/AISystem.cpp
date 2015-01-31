/*********************************
 * AISystem.cpp
 * Connor Hilarides
 * Created 2015/01/16
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "AISystem.h"
#include "WorldSnapshot.h"

// ----------------------------------------------------------------------------

static std::unordered_map<std::string, AIFactory *> factories;

void AIFactory::Register(std::string name, AIFactory *factory)
{
  factories[name] = factory;
}

// ----------------------------------------------------------------------------

AIDecisionRef AIDecision::New(AIFactory &factory, json::value params)
{
  AIDecision *decision = nullptr;
  AIDecisionRef ref;

  try
  {
    decision = new AIDecision(factory, params);
    ref = AIDecisionRef(decision);
  }
  catch (...)
  {
    delete decision;
    throw;
  }

  return ref;
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

void AIDecision::Init(Entity *owner, Entity *target)
{
  behavior->InitilizeOwner(owner);
  behavior->InitializeTarget(target);
}

// ----------------------------------------------------------------------------

void AIDecision::Run(const WorldSnapshot& snap, json::value params)
{
  behavior->ApplyBehaviour(snap, params);
}

// ----------------------------------------------------------------------------

AIDecision::AIDecision(AIFactory &factory, json::value params)
  : behavior(factory.Create(params)), hasResult(false)
{
}

// ----------------------------------------------------------------------------

AISystem::AISystem(size_t threadCount)
{
  while (threadCount)
  {
    decisionThreads.emplace_back(std::bind(&AISystem::RunThread, this));
  }
}

// ----------------------------------------------------------------------------

AISystem::~AISystem()
{
  quit = true;

  for (auto &thread : decisionThreads)
  {
    thread.join();
  }
}

// ----------------------------------------------------------------------------

AIDecisionRef AISystem::QueueBehavior(AIFactory& factory, json::value params,
                                      Entity *owner, Entity *target)
{
  auto decision = AIDecision::New(factory, params);
  decision->Init(owner, target);

  auto queue = decisionQueue.deref_mut();
  queue->push(decision);

  return decision;
}

// ----------------------------------------------------------------------------

void AISystem::UpdateSnapshot()
{
  auto snapshot = this->snapshot.write();
  *snapshot = WorldSnapshot();
}

// ----------------------------------------------------------------------------

void AISystem::RunThread()
{
  while (!quit)
  {
    auto decision = GetDecision();
    auto snapshot = this->snapshot.read();

    decision->Run(*snapshot, decision->params);

    decision->result = decision->behavior->GetResult();
    decision->hasResult = true;
  }
}

// ----------------------------------------------------------------------------

AIDecisionRef AISystem::GetDecision()
{
  for(;;)
  {
    Sleep(1);

    auto queue = decisionQueue.deref_mut();
    if (!queue->empty())
    {
      auto decision = queue->front();
      queue->pop();
      return decision;
    }
  }
}

// ----------------------------------------------------------------------------
