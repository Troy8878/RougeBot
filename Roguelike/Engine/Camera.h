/*********************************
 * Camera.h
 * Connor Hilarides
 * Created 2014/07/03
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
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

struct ICamera : Camera
{
  virtual ~ICamera() {}

  virtual void Init() = 0;
  virtual void Update() = 0;

  virtual void LoadFromData(const component_factory_data &data) = 0;
  virtual mrb_value GetRubyWrapper() = 0;
};

// ----------------------------------------------------------------------------

struct Basic3DCamera : ICamera
{
  math::Vector position;
  math::Vector rotation;
  float fieldOfView = math::degrees(45);
  float aspectRatio = 1280.f / 720.f;
  float nearField = 0.1f, farField = 100;

  void Init() override
  {
    using namespace DirectX;
    projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio,
                                                nearField, farField);
  }

  void Update() override
  {
    using namespace DirectX;
    auto rotate = XMMatrixRotationRollPitchYawFromVector(rotation.get());

    auto up = rotate * g_XMIdentityR1; // [rotate] * <0, 1, 0, 0>
    auto lookAt = position + rotate * g_XMIdentityR2; // <position> + [rotate] * <0, 0, 1, 0>

    viewMatrix = XMMatrixLookAtLH(position, lookAt, up);
  }

  void LoadFromData(const component_factory_data &data) override;
  mrb_value GetRubyWrapper() override;
};

// ----------------------------------------------------------------------------

struct LookAtCamera : ICamera
{
  math::Vector position;
  math::Vector lookAt;
  float fieldOfView = math::pi / 4;
  float aspectRatio = 1280.f / 720.f;
  float nearField = 0.1f, farField = 100;

  void Init() override
  {
    using namespace DirectX;
    projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearField, farField);
  }

  void Update() override
  {
    using namespace DirectX;
    auto pos = position.get();
    auto look = lookAt.get();
    if (XMVector3Equal(look, pos))
      look = pos + g_XMIdentityR2;

    viewMatrix = XMMatrixLookAtLH(pos, look, g_XMIdentityR2);
  }

  void LoadFromData(const component_factory_data &data) override;
  mrb_value GetRubyWrapper() override;
};

// ----------------------------------------------------------------------------

struct HUDCamera final : ICamera
{
  math::Vector position = {0, 0, 1, 1};
  math::Vector2D size = {1280.f / 720.f, 1};
  float nearField = 0.1f, farField = 100;

  void Init() override
  {
    using namespace DirectX;
    projectionMatrix = XMMatrixOrthographicLH(size.x, size.y, nearField, farField);
  }

  void Update() override
  {
    using namespace DirectX;
    viewMatrix = XMMatrixLookToLH(position.get(), g_XMIdentityR2, g_XMIdentityR1);
  }

  void LoadFromData(const component_factory_data &data) override;
  mrb_value GetRubyWrapper() override;
};

// ----------------------------------------------------------------------------

struct ManualCamera final : ICamera
{
  math::Matrix *cameraTransform = nullptr;
  float fieldOfView = math::pi / 4;
  float aspectRatio = 1280.f / 720.f;
  float nearField = 0.1f, farField = 100;

  void Init() override
  {
    using namespace DirectX;
    projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearField, farField);
  }

  void Update() override
  {
    using namespace DirectX;
    XMMATRIX transfrom = *cameraTransform;

    XMVECTOR position = transfrom * g_XMIdentityR3; // [transform] * <0, 0, 0, 1>
    XMVECTOR forward = transfrom * g_XMIdentityR2; // [transform] * <0, 0, 1, 0>
    XMVECTOR up = transfrom * g_XMIdentityR1; // [transform] * <0, 1, 0, 0>

    viewMatrix = XMMatrixLookToLH(position, forward, up);
  }

  void LoadFromData(const component_factory_data &data) override;
  mrb_value GetRubyWrapper() override;
};

// ----------------------------------------------------------------------------

#pragma warning (disable : 4324) // I know it added that padding :U


__declspec(align(16)) struct MultiCam
{
  MultiCam()
  {
  }

  __declspec(align(16)) union
  {
    byte buffer[sizeof(ICamera)];
    byte _b3buffer[sizeof(Basic3DCamera)];
    byte _labuffer[sizeof(LookAtCamera)];
    byte _hdbuffer[sizeof(HUDCamera)];
    byte _mcbuffer[sizeof(ManualCamera)];
  };

  std::type_index type = typeid(ICamera);

  __declspec(property(get = GetBase)) ICamera *Base;

  template <typename CamType>
  void SetType()
  {
    type = typeid(CamType);
    new(GetCamera<CamType>()) CamType;
  }

  template <typename CamType>
  CamType *GetCamera()
  {
    return static_cast<CamType *>(Base);
  }

  ICamera *GetBase()
  {
    return reinterpret_cast<ICamera *>(buffer);
  }
};

#pragma warning (default : 4324) // restore


// ----------------------------------------------------------------------------