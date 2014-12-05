/*********************************
 * ParticleSystemComponent.h
 * Connor Hilarides, Enrique Rodriguez
 * Created 2014/09/30
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
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
  ParticleSystemComponent::ParticleSystemComponent(size_t maxParticles, RenderSet *target,
    const math::Vector &scale, const math::Vector &rotation, const math::Vector &velocity,
    const math::Vector &rotVel, const math::Vector &_offSet, float rate, float fade,
    bool active);
  ~ParticleSystemComponent();

  void Initialize(Entity *owner, const std::string &name) override;
  void OnUpdate(Events::EventMessage &e);
  void OnCall();

  void Draw() override;

  void OnSetDestroyed() override
  {
    renderTarget = nullptr;
  }

  mrb_value GetRubyWrapper() override;

  static ParticleSystemComponentFactory factory;

  float GetScale(int index);
  float GetRotation(int index);
  float GetVelocity(int index);
  float GetRotationVelocity(int index);
  float GetOffSet(int index);
  float GetParticleRate();
  float GetFadeTime();
  bool GetActive();

  void SetScale(math::Vector ranges);
  void SetRotation(math::Vector ranges);
  void SetVelocity(math::Vector ranges);
  void SetRotationVelocity(math::Vector ranges);
  void SetOffSet(math::Vector translation);
  void SetParticleRate(float rate);
  void SetFadeTime(float time);
  void SetActive(bool _active);

  math::Vector scaleRange, rotationRange, velocityRange, rotVelRange, offSet;
  float particleRate, fadeTime;
  bool active;
  ParticleSystem system;

private:
  RenderSet *renderTarget = nullptr;

  static Model *GetUnitSquare();
};

// ----------------------------------------------------------------------------

class ParticleSystemComponentFactory final : public IComponentFactory
{
public:
  ParticleSystemComponentFactory();

  Component *CreateObject(void *memory, component_factory_data &data) override;

  IAllocator *Allocator() override
  {
    return &allocator;
  }

private:
  BucketAllocator allocator;

  math::Vector ParseVector(json::value jv);
  float ParseFloat(json::value jv);
  bool ParseBool(json::value jv);
};

// ----------------------------------------------------------------------------