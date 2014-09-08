/*********************************
 * CameraComponent.h
 * Connor Hilarides
 * Created 2014/09/06
 *********************************/

#pragma once

#include "Common.h"
#include "Camera.h"

#include "Helpers\BucketAllocator.h"

// ----------------------------------------------------------------------------

class CameraComponentFactory;

// ----------------------------------------------------------------------------

class CameraComponent : public Component
{
public:
  CameraComponent(const std::string& name, int layer, const MultiCam& camera);
  ~CameraComponent();

  PROPERTY(get = _GetICamera) ICamera *Camera;
  IR_PROPERTY(std::string, Name);

  static CameraComponentFactory factory;

private:
  MultiCam _camera;

public:
  ICamera *_GetICamera() { return _camera.GetCamera<ICamera>(); }
};

// ----------------------------------------------------------------------------

class CameraComponentFactory : IComponentFactory
{
public:
  CameraComponentFactory();

  Component *CreateObject(void *memory, component_factory_data& data) override;
  IAllocator *_GetAllocator() override { return &allocator; }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------
