/*********************************
 * TestcompyComponent.h
 * Troy
 * Created 2014/09/15
 *********************************/

#pragma once

#include "Common.h"
#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

class TestcompyComponentFactory;
class TransformComponent;

// ----------------------------------------------------------------------------

class TestcompyComponent : public Component
{
public:
  TestcompyComponent();

  void Initialize(Entity *owner, const std::string& name) override;

  void OnUpdate(Events::EventMessage& e);

  mrb_value GetRubyWrapper() override;

  static TestcompyComponentFactory factory;
  
private:
  TransformComponent *_transform;

};

// ----------------------------------------------------------------------------

class TestcompyComponentFactory : public IComponentFactory
{
public:
  TestcompyComponentFactory();

  Component *CreateObject(void *memory, component_factory_data& data) override;
  IAllocator *_GetAllocator() override { return &allocator; }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------
