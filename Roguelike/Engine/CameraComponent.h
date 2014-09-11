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
  CameraComponent(const std::string& name, int layer, MultiCam *copy);
  ~CameraComponent();

  void Initialize(Entity *owner, const std::string& name) override;
  
  IR_PROPERTY(std::string, Name);
  PROPERTY(get = _GetICamera) ICamera *Camera;
  PROPERTY(get = _GetMCamera) MultiCam& MCamera;
  PROPERTY(get = _GetCameraType) std::type_index CameraType;

  void FixCameras();
  void OnUpdate(Events::EventMessage&);
  mrb_value GetRubyWrapper() override { return Camera->GetRubyWrapper(); }

  static CameraComponentFactory factory;

private:
  MultiCam _camera;
  MultiCam *_cameraPtr;
  math::Vector2D prevSize = {0, 0};

public:
  inline ICamera *_GetICamera() { return _cameraPtr->GetCamera<ICamera>(); }
  inline MultiCam& _GetMCamera() { return *_cameraPtr; }
  inline const std::type_index& _GetCameraType() { return _cameraPtr->type; }
};

// ----------------------------------------------------------------------------

class CameraComponentFactory : public IComponentFactory
{
public:
  CameraComponentFactory();

  Component *CreateObject(void *memory, component_factory_data& data) override;
  IAllocator *_GetAllocator() override { return &allocator; }

private:
  BucketAllocator allocator;
};

// ----------------------------------------------------------------------------
