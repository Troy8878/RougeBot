/*********************************
 * ParticleSystemComponent.cpp
 * YOUR NAME HERE
 * Created 2014/YOUR DATE HERE
 *********************************/

#include "Common.h"
#include "ParticleSystemComponent.h"
#include "StandardShapes.h"
#include "Shader.h"

// ----------------------------------------------------------------------------

ParticleSystemComponentFactory ParticleSystemComponent::factory;

// ----------------------------------------------------------------------------

ParticleSystemComponent::ParticleSystemComponent(size_t maxParticles)
  : system(maxParticles)
{
  system.model = GetUnitSquare();
  system.shader = RegisteredShaders["Textured"];
}

// ----------------------------------------------------------------------------

void ParticleSystemComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);

  DEF_EVENT_ID(update);
  Owner->AddEvent(this, update, &ParticleSystemComponent::OnUpdate);
}

// ----------------------------------------------------------------------------

Model *ParticleSystemComponent::GetUnitSquare()
{
  static Model *square = nullptr;

  if (!square)
    square = Shapes::MakeRectangle(GetGame()->GameDevice->Device, {0.1f, 0.1f});

  return square;
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
