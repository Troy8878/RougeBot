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

struct ICamera : Camera
{
  virtual void Init() = 0;
  virtual void Update() = 0;

  virtual void LoadFromData(const component_factory_data& data) = 0;
  virtual mrb_value GetRubyWrapper() = 0;
};

// ----------------------------------------------------------------------------

struct Basic3DCamera : ICamera
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

  void LoadFromData(const component_factory_data& data) override;
  mrb_value GetRubyWrapper() override;
};

// ----------------------------------------------------------------------------

struct LookAtCamera : ICamera
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

  void LoadFromData(const component_factory_data& data) override;
  mrb_value GetRubyWrapper() override;
};

// ----------------------------------------------------------------------------

struct HUDCamera : ICamera
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

  void LoadFromData(const component_factory_data& data) override;
  mrb_value GetRubyWrapper() override;
};

// ----------------------------------------------------------------------------

struct MultiCam
{
  union
  {
    byte buffer[sizeof(ICamera)];
    byte _b3buffer[sizeof(Basic3DCamera)];
    byte _labuffer[sizeof(LookAtCamera)];
    byte _hdbuffer[sizeof(HUDCamera)];
  };

  std::type_index type = typeid(ICamera);

  template <typename CamType>
  void SetType()
  {
    GetCamera<CamType>()->CamType::CamType();
  }
  
  template <typename CamType>
  CamType *GetCamera() 
  {
    auto icam = reinterpret_cast<ICamera *>(buffer);
    return static_cast<CamType *>(icam); 
  }
};

// ----------------------------------------------------------------------------

