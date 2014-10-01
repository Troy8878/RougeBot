/*********************************
 * ParticleSystemComponent.h
 * Connor Hilarides
 * Created 2014/09/30
 *********************************/

#pragma once

#include "Common.h"
#include "Helpers\BucketAllocator.h"
#include "ParticleSystem.h"

// ----------------------------------------------------------------------------

class ParticleSystemComponentFactory;

// ----------------------------------------------------------------------------

class ParticleSystemComponent : public Component
{
public:
  ParticleSystemComponent(size_t maxParticles);
  ~ParticleSystemComponent();

  ParticleSystem system;

  void Initialize(Entity *owner, const std::string& name) override;

  mrb_value GetRubyWrapper() override;

  static ParticleSystemComponentFactory factory;
};

// ----------------------------------------------------------------------------

class ParticleSystemComponentFactory : public IComponentFactory
{
public:
  ParticleSystemComponentFactory();

  Component *CreateObject(void *memory, component_factory_data& data) override;
  IAllocator *_GetAllocator() override { return &allocator; }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------
