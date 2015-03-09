/*********************************
 * ParticleSystem.cpp
 * Enrique Rodriguez
 * Created 2014/09/29
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "ParticleSystem.h"
#include "Model.h"
#include "Shader.h"

// ----------------------------------------------------------------------------

ParticleSystem::ParticleSystem(size_t maxParticles)
  : particleCount(maxParticles), activeParticles(0)
{
  particles = new Particle[particleCount];
  freeParticle = particles;

  for (auto &particle : array_iterator(particles, particleCount))
    particle.inUse = false;
}

// ----------------------------------------------------------------------------

ParticleSystem::~ParticleSystem()
{
  delete particles;
}

// ----------------------------------------------------------------------------

bool ParticleSystem::SpawnParticle(DirectX::FXMMATRIX initial, double life)
{
  if (activeParticles == particleCount)
    return false;

  auto &particle = *freeParticle;
  particle.transform = particleTransform;
  particle.world = initial;
  particle.life = life;
  particle.inUse = true;
  ++activeParticles;

  if (activeParticles <= particleCount)
    freeParticle = NextFreeParticle(freeParticle);

  return true;
}

// ----------------------------------------------------------------------------

void ParticleSystem::Update(float dt)
{
  using namespace DirectX;

  size_t iterated = 0;
  for (auto &particle : array_iterator(particles, particleCount))
  {
    if (!particle.inUse)
      continue;

    if (++iterated > activeParticles)
      break;

    XMMATRIX suffixTransform =
      XMMatrixRotationRollPitchYawFromVector(particle.transform.rotationalVelocity * dt) *
      XMMatrixTranslationFromVector(particle.transform.absoluteVelocity * dt);
    XMMATRIX prefixTransform =
      XMMatrixScalingFromVector(g_XMOne + particle.transform.scaleRate * dt) *
      XMMatrixRotationRollPitchYawFromVector(particle.transform.rotationRate * dt);

    particle.life -= dt;
    particle.world = prefixTransform * particle.world * suffixTransform;

    if (particle.life <= 0)
      KillParticle(&particle);
  }
}

// ----------------------------------------------------------------------------

void UpdateTint(ID3D11Buffer *buffer, const math::Vector &tint);
extern ID3D11Buffer *tintRes;

void ParticleSystem::Draw()
{
  model->texture = texture;

  // I know it's copied from Model::Draw and Shader::Draw,
  // but I need to improve the performance
  // and only push this stuff once per particle system
  unsigned offset = 0;
  auto *vb = model->GetVB();
  auto *ib = model->GetIB();
  auto stride = model->GetStride();

  auto context = shader->device->DeviceContext;
  context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
  context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, offset);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  static auto nulltex = Texture2D::GetNullTexture();

  if (model->texture && !shader->device->WireframeDraw)
    context->PSSetShaderResources(0, 1, &model->texture.ShaderRes);
  else
    context->PSSetShaderResources(0, 1, &nulltex.ShaderRes);

  context->VSSetConstantBuffers(0, 1, &shader->cameraBuffer);
  context->IASetInputLayout(shader->vertexLayout);
  context->VSSetShader(shader->vertexShader, nullptr, 0);
  context->PSSetShader(shader->pixelShader, nullptr, 0);

  using namespace DirectX;
  XMMATRIX view = XMMatrixTranspose(camera->viewMatrix);
  XMMATRIX proj = XMMatrixTranspose(camera->projectionMatrix);

  size_t iterated = 0;
  for (auto &particle : array_iterator(particles, particleCount))
  {
    if (!particle.inUse)
      continue;

    if (++iterated > activeParticles)
      break;

    DrawParticle(particle, view, proj);
  }
}

// ----------------------------------------------------------------------------

void ParticleSystem::DrawParticle(const Particle &particle,
                                  DirectX::CXMMATRIX view,
                                  DirectX::CXMMATRIX proj)
{
  using namespace DirectX;
  auto context = shader->device->DeviceContext;
  HRESULT result;

  D3D11_MAPPED_SUBRESOURCE mappedCameraRes;
  result = context->Map(shader->cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD,
                        0, &mappedCameraRes);
  CHECK_HRESULT(result);

  Camera *mCamera = static_cast<Camera *>(mappedCameraRes.pData);

  mCamera->worldMatrix = XMMatrixTranspose(particle.world);
  mCamera->viewMatrix = view;
  mCamera->projectionMatrix = proj;

  context->Unmap(shader->cameraBuffer, 0);

  math::Vector tint = { 1, 1, 1, 1 };

  if (particle.life < particle.transform.fadeTime)
    tint.w = tint.w * static_cast<float>(particle.life / particle.transform.fadeTime / 2 + 0.5);

  UpdateTint(tintRes, tint);
  context->PSSetConstantBuffers(0, 1, &tintRes);

  context->DrawIndexed(model->GetIC(), 0, 0);
}

// ----------------------------------------------------------------------------

auto ParticleSystem::NextFreeParticle(Particle *current) -> Particle *
{
  Particle *iter = current + 1;
  Particle *end = current;
  Particle *last = particles + particleCount;

  while (iter != end)
  {
    if (iter == last)
      iter = particles;

    if (!iter->inUse)
      return iter;

    iter++;
  }

  return nullptr;
}

// ----------------------------------------------------------------------------

void ParticleSystem::KillParticle(Particle *particle)
{
  --activeParticles;
  particle->inUse = false;
  freeParticle = particle;
}

// ----------------------------------------------------------------------------