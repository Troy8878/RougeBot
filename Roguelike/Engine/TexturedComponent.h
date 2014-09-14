/*********************************
 * TexturedComponent.h
 * YOUR NAME HERE
 * Created 2014/YOUR DATE HERE
 *********************************/

// Just Ctrl+H "Textured" with your component name

#pragma once

#include "Common.h"
#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

class TexturedComponentFactory;

// ----------------------------------------------------------------------------

class TexturedComponent : public Component
{
public:
  TexturedComponent();

  void Initialize(Entity *owner, const std::string& name) override;

  mrb_value GetRubyWrapper() override;

  static TexturedComponentFactory factory;
};

// ----------------------------------------------------------------------------

class TexturedComponentFactory : public IComponentFactory
{
public:
  TexturedComponentFactory();

  Component *CreateObject(void *memory, component_factory_data& data) override;
  IAllocator *_GetAllocator() override { return &allocator; }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------
