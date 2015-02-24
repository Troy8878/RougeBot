/*********************************
 * AISystem.cpp
 * Connor Hilarides
 * Created 2015/01/16
 * Copyright � 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "AISystem.h"
#include "WorldSnapshot.h"

// ----------------------------------------------------------------------------

static std::unordered_map<std::string, AIFactory *> factories;

// ----------------------------------------------------------------------------

void AIFactory::Register(const std::string &name, AIFactory *factory)
{
  factories.emplace(std::make_pair(name, factory));
}

// ----------------------------------------------------------------------------

AIFactory *AIFactory::GetFactory(const std::string &name)
{
  return factories[name];
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
  : behavior(factory.Create()), hasResult(false), params(params)
{
}

// ----------------------------------------------------------------------------

AISystem::AISystem(size_t threadCount)
{
  AIFactory::RegisterDefaultFactories();

  while (threadCount--)
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

mrb_data_type mrb_aisys_dt;

static mrb_value mrb_aisys_instance(mrb_state *mrb, mrb_value klass);

// ----------------------------------------------------------------------------

EXTERN_C void mrb_mruby_aisys_init(mrb_state *mrb)
{
  ruby::data_type_init<AISystem>(mrb_aisys_dt);
  auto aclass = mrb_define_class(mrb, "AISystem", mrb->object_class);

  mrb_define_class_method(mrb, aclass, "instance", mrb_aisys_instance, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, aclass, "new", mrb_nop, MRB_ARGS_NONE());
}

// ----------------------------------------------------------------------------

static mrb_value mrb_aisys_instance(mrb_state *mrb, mrb_value klass)
{
  auto klassp = mrb_class_ptr(klass);
  auto datap = GetGame()->AI;
  auto data = mrb_data_object_alloc(mrb, klassp, datap, &mrb_aisys_dt);
  return mrb_obj_value(data);
}

// ----------------------------------------------------------------------------

#include "AIDerp.h"

void AIFactory::RegisterDefaultFactories()
{
  Register("AIDerp", new DefaultAIFactory<AIDerp>);
}

// ----------------------------------------------------------------------------