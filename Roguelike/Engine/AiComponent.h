/*********************************
 * AiComponent.h
 * YOUR NAME HERE
 * Created 2015/YOUR DATE HERE
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

// Just Ctrl+H "Ai" with your component name

#pragma once

#include "Common.h"
#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

class AiComponentFactory;

// ----------------------------------------------------------------------------

class AiComponent : public Component
{
public:
  AiComponent();

  void Initialize(Entity *owner, const std::string &name) override;
  void OnUpdate(Events::EventMessage &);

  mrb_value GetRubyWrapper() override;
  void AddDecision(AIDecisionRef ref) { decisions.push(ref); }

  static AiComponentFactory factory;

private:
  std::queue<AIDecisionRef> decisions;
};

// ----------------------------------------------------------------------------

class AiComponentFactory final : public IComponentFactory
{
public:
  AiComponentFactory();

  Component *CreateObject(void *memory, component_factory_data &data) override;

  IAllocator *Allocator() override
  {
    return &allocator;
  }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------
