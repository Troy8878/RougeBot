/*********************************
 * ButtonComponent.h
 * Leonardo Saikali
 * Created 2014/10/07
 *********************************/

#pragma once

#include "Common.h"
#include "Helpers\BucketAllocator.h"
#include "RenderSet.h"

// ----------------------------------------------------------------------------

class ButtonComponentFactory;

// ----------------------------------------------------------------------------

class ButtonComponent : public Component
{
public:
  ButtonComponent();

  void Initialize(Entity *owner, const std::string& name) override;
  void OnUpdate(Events::EventMessage&);
  void OnProbe(Events::EventMessage& e);

  RenderSet *RenderTarget;
  math::Vector Size;

  mrb_value GetRubyWrapper() override;

  static ButtonComponentFactory factory;
};

// ----------------------------------------------------------------------------

class ButtonComponentFactory : public IComponentFactory
{
public:
  ButtonComponentFactory();

  Component *CreateObject(void *memory, component_factory_data& data) override;
  IAllocator *_GetAllocator() override { return &allocator; }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------
