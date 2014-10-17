/*********************************
 * ParticleSystemComponent.cpp
 * Connor Hilarides
 * Created 2014/09/30
 *********************************/

#include "Common.h"
#include "ParticleSystemComponent.h"
#include "StandardShapes.h"
#include "Shader.h"

// ----------------------------------------------------------------------------

ParticleSystemComponentFactory ParticleSystemComponent::factory;

// ----------------------------------------------------------------------------

ParticleSystemComponent::ParticleSystemComponent(size_t maxParticles, RenderSet *target)
  : system(maxParticles), renderTarget(target)
{
  system.model = GetUnitSquare();
  system.shader = RegisteredShaders["Textured"];
  system.camera = renderTarget->RenderCamera;

  system.particleTransform.absoluteVelocity = math::Vector{0, 0.5f, 1.1f, 0};
  system.particleTransform.scaleRate = math::Vector{0.5f, 0.5f, 0.5f, 0};
}

// ----------------------------------------------------------------------------

ParticleSystemComponent::~ParticleSystemComponent()
{
  if (renderTarget)
    renderTarget->RemoveDrawable(this);
}

// ----------------------------------------------------------------------------

void ParticleSystemComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);

  DEF_EVENT_ID(update);
  Owner->AddEvent(this, update, &ParticleSystemComponent::OnUpdate);

  renderTarget->AddDrawable(this, system.shader);
}

// ----------------------------------------------------------------------------

void ParticleSystemComponent::OnUpdate(Events::EventMessage& e)
{
  float dt = (float) e.GetData<Events::UpdateEvent>()->gameTime.Dt;

  static double tb = 0;
  tb += dt;
  while (tb > 0.01f)
  {
    system.SpawnParticle(Owner->Transform, 1);
    tb -= 0.1;
  }

  system.Update(dt);
}

// ----------------------------------------------------------------------------

void ParticleSystemComponent::Draw()
{
  system.Draw();
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
  auto targetName = data["render_target"].as_string();
  auto target = RenderGroup::Instance.GetSet(targetName);

  auto *component = new (memory) ParticleSystemComponent(1000, target);

  return component;
}

// ----------------------------------------------------------------------------

mrb_value ParticleSystemComponent::GetRubyWrapper()
{
  ONE_TIME_MESSAGE("[WARN] TODO: Implement ruby wrapper for ParticleSystem");
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------
