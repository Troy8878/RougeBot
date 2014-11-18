/*********************************
 * ButtonComponent.h
 * Leonardo Saikali
 * Created 2014/10/07
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#pragma once

#include "Common.h"
#include "Helpers\BucketAllocator.h"
#include "RenderSet.h"

// ----------------------------------------------------------------------------

class ButtonComponentFactory;
class SpriteComponent;

// ----------------------------------------------------------------------------

class ButtonComponent : public Component
{
public:
  ButtonComponent();

  void Initialize(Entity *owner, const std::string &name) override;
  void OnUpdate(Events::EventMessage &);
  void OnProbe(Events::EventMessage &e);

  RenderSet *RenderTarget;
  math::Vector Size;
  SpriteComponent *Sprite;

  mrb_value GetRubyWrapper() override;

  static ButtonComponentFactory factory;
};

// ----------------------------------------------------------------------------

class ButtonComponentFactory final : public IComponentFactory
{
public:
  ButtonComponentFactory();

  Component *CreateObject(void *memory, component_factory_data &data) override;

  IAllocator *Allocator() override
  {
    return &allocator;
  }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------