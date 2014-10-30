/*********************************
 * ParticleSystemComponent.h
 * Connor Hilarides, Enrique Rodriguez
 * Created 2014/09/30
 *********************************/

#pragma once

#include "Common.h"
#include "Helpers\BucketAllocator.h"
#include "ParticleSystem.h"
#include "RenderSet.h"

struct Ranges
{
  float Xmin;
  float Xmax;
  float Ymin;
  float Ymax;
};

// ----------------------------------------------------------------------------

class ParticleSystemComponentFactory;

// ----------------------------------------------------------------------------

class ParticleSystemComponent : public Component, public Drawable
{
public:
  ParticleSystemComponent(size_t maxParticles, RenderSet *target, Ranges& scale, Ranges& rotation,
                          Ranges& velocity, Ranges& rotVel, float rate);
  ~ParticleSystemComponent();

  void Initialize(Entity *owner, const std::string& name) override;
  void OnUpdate(Events::EventMessage&);
  
  void Draw() override;
  void OnSetDestroyed() override { renderTarget = nullptr; }

  mrb_value GetRubyWrapper() override;

  static ParticleSystemComponentFactory factory;

  float GetScale(int index);
  float GetRotation(int index);
  float GetVelocity(int index);
  float GetRotationVelocity(int index);
  void SetScale(float Xmin, float Xmax, float Ymin, float Ymax);
  void SetRotation(float Xmin, float Xmax, float Ymin, float Ymax);
  void SetVelocity(float Xmin, float Xmax, float Ymin, float Ymax);
  void SetRotationVelocity(float Xmin, float Xmax, float Ymin, float Ymax);

private:
  ParticleSystem system;
  RenderSet *renderTarget = nullptr;

  struct Ranges scaleRange, rotationRange, velocityRange, rotVelRange;
  float ParticleRate;

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

  Ranges ParseRanges(json::value jv);
  float ParseFloat(json::value jv);
};

// ----------------------------------------------------------------------------
