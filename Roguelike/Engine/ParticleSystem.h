/*********************************
 * ParticleSystem.h
 * Connor Hilarides
 * Created 2014/09/29
 *********************************/

#pragma once

#include "Common.h"

// ----------------------------------------------------------------------------

struct Shader;
class Model;

// ----------------------------------------------------------------------------

class ParticleSystem
{
public:
  explicit ParticleSystem(size_t maxParticles);
  ~ParticleSystem();

  Shader *shader;
  Model *model;
  Camera *camera;

  struct ParticleTransform
  {
    math::Vector scaleRate = {0, 0, 0, 0};
    math::Vector rotationRate = {0, 0, 0, 0};
    math::Vector absoluteVelocity = {0, 0, 0, 0};
    math::Vector rotationalVelocity = {0, 0, 0, 0};
  } particleTransform;

  bool SpawnParticle(DirectX::FXMMATRIX initial, double life);

  void Update(float dt);
  void Draw();

private:
  struct Particle
  {
    math::Matrix world;
    double life;
    bool inUse;
  };

  size_t activeParticles;
  size_t particleCount;
  Particle *particles;
  Particle *freeParticle;

  void DrawParticle(const Particle &particle, DirectX::CXMMATRIX view, DirectX::CXMMATRIX proj);

  Particle *NextFreeParticle(Particle *current);
  void KillParticle(Particle *current);
};

// ----------------------------------------------------------------------------