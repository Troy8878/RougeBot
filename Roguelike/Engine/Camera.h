/*********************************
 * Camera.h
 * Connor Hilarides
 * Created 2014/07/03
 *********************************/

#pragma once

#include "Helpers\UseDirectX.h"
#include "Helpers\Console.h"

// ----------------------------------------------------------------------------

struct Camera
{
  math::Matrix worldMatrix;
  math::Matrix viewMatrix;
  math::Matrix projectionMatrix;
};

// ----------------------------------------------------------------------------

struct Basic3DCamera : Camera
{
  math::Vector position;
  math::Vector rotation;
  float fieldOfView = math::degrees(45);
  float aspectRatio = 1280.f / 720.f;
  float nearField = 0.1f, farField = 10000;

  void Init()
  {
    using namespace DirectX;
    projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, 
                                                nearField, farField);
  }

  void Update()
  {
    using namespace DirectX;
    auto rotate = XMMatrixRotationRollPitchYawFromVector(rotation.get());

    auto up = g_XMIdentityR1; // <0, 1, 0, 0>
    auto lookAt = position.get() + rotate * g_XMIdentityR2; // <position> + [rotate] * <0, 0, 1, 0>

    viewMatrix = XMMatrixLookAtLH(position.get(), lookAt, up);
  }
};

// ----------------------------------------------------------------------------

struct LookAtCamera : Camera
{
  math::Vector position;
  math::Vector lookAt;
  float fieldOfView = math::pi / 4;
  float aspectRatio = 1280.f / 720.f;
  float nearField = 0.1f, farField = 10000;

  void Init()
  {
    using namespace DirectX;
    projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearField, farField);
  }

  void Update()
  {
    using namespace DirectX;
    viewMatrix = XMMatrixLookAtLH(position.get(), lookAt.get(), g_XMIdentityR1);
  }
};

struct HUDCamera : Camera
{
  math::Vector position = {0, 0, 1, 1};
  math::Vector2D size = {1280.f / 720.f, 1};
  float nearField = 0.1f, farField = 10000;

  void Init()
  {
    using namespace DirectX;
    projectionMatrix = XMMatrixOrthographicLH(size.x, size.y, nearField, farField);
  }

  void Update()
  {
    using namespace DirectX;
    viewMatrix = XMMatrixLookToLH(position.get(), g_XMIdentityR2, g_XMIdentityR1);
  }
};

// ----------------------------------------------------------------------------

