/*********************************
 * ParticleSystemComponent.h
 * Connor Hilarides
 * Created 2014/09/30
 *********************************/

#pragma once

#include "Common.h"
#include "Helpers\BucketAllocator.h"
#include "ParticleSystem.h"
#include "RenderSet.h"

// ----------------------------------------------------------------------------

class ParticleSystemComponentFactory;

// ----------------------------------------------------------------------------

class ParticleSystemComponent : public Component, public Drawable
{
public:
  ParticleSystemComponent(size_t maxParticles, RenderSet *target);
  ~ParticleSystemComponent();

  void Initialize(Entity *owner, const std::string& name) override;
  void OnUpdate(Events::EventMessage&);
  
  void Draw() override;
  void OnSetDestroyed() override { renderTarget = nullptr; }

  mrb_value GetRubyWrapper() override;

  static ParticleSystemComponentFactory factory;

private:
  ParticleSystem system;
  RenderSet *renderTarget = nullptr;

  static Model *GetUnitSquare();
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
