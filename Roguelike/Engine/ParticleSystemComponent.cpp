/*********************************
 * ParticleSystemComponent.cpp
 * YOUR NAME HERE
 * Created 2014/YOUR DATE HERE
 *********************************/

#include "Common.h"
#include "ParticleSystemComponent.h"

// ----------------------------------------------------------------------------

ParticleSystemComponentFactory ParticleSystemComponent::factory;

// ----------------------------------------------------------------------------

ParticleSystemComponent::ParticleSystemComponent(size_t maxParticles)
  : system(maxParticles)
{
}

// ----------------------------------------------------------------------------

void ParticleSystemComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);
}

// ----------------------------------------------------------------------------

ParticleSystemComponentFactory::ParticleSystemComponentFactory()
  : allocator(sizeof(ParticleSystemComponent))
{
}

// ----------------------------------------------------------------------------

Component *ParticleSystemComponentFactory::CreateObject(
  void *memory, component_factory_data& data)
{
  (data); // do something with the serialization data

  auto *component = new (memory) ParticleSystemComponent(1000);

  // do something to the component

  return component;
}

// ----------------------------------------------------------------------------

mrb_value ParticleSystemComponent::GetRubyWrapper()
{
  ONE_TIME_MESSAGE("[WARN] TODO: Implement ruby wrapper for ParticleSystem");
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------
