/*********************************
 * CameraComponent.cpp
 * Connor Hilarides
 * Created 2014/09/06
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "CameraComponent.h"
#include "RenderSet.h"

// ----------------------------------------------------------------------------

CameraComponentFactory CameraComponent::factory;

// ----------------------------------------------------------------------------

CameraComponent::CameraComponent(const std::string &name, int layer, const MultiCam &camera)
  : _Name(name), _camera(camera), _cameraPtr(&_camera)
{
  RenderGroup::Instance.CreateSet(name, Camera, layer, false);
}

// ----------------------------------------------------------------------------

CameraComponent::CameraComponent(const std::string &name, int layer, MultiCam *copy)
  : _Name(name), _cameraPtr(copy)
{
  if (!_cameraPtr)
    throw basic_exception("Camera copy pointer was null D:");

  RenderGroup::Instance.CreateSet(name, Camera, layer, false);
}

// ----------------------------------------------------------------------------

CameraComponent::~CameraComponent()
{
  RenderGroup::Instance.RemoveSet(Name);
}

// ----------------------------------------------------------------------------

void CameraComponent::Initialize(Entity *owner, const std::string &name)
{
  Component::Initialize(owner, name);

  DEF_EVENT_ID(draw);
  Owner->AddEvent(this, draw, &CameraComponent::OnUpdate);
}

// ----------------------------------------------------------------------------

void CameraComponent::FixCameras()
{
  // Copy cameras, don't do this logic, it's already been applied
  if (_cameraPtr != &_camera)
    return;

  auto newSize = GetGame()->GameDevice->GetSize();
  if (prevSize != newSize)
  {
    prevSize = newSize;

    // Mostly just fixing the aspect ratios
    if (CameraType == typeid(HUDCamera))
    {
      auto &hcam = *MCamera.GetCamera<HUDCamera>();
      hcam.size.x = hcam.size.y * newSize.x / newSize.y;
      Camera->Init();
    }
    else if (CameraType == typeid(LookAtCamera))
    {
      auto &lcam = *MCamera.GetCamera<LookAtCamera>();
      lcam.aspectRatio = newSize.x / newSize.y;
      Camera->Init();
    }
    else if (CameraType == typeid(ManualCamera))
    {
      // but also letting ManualCamera know its owner transform
      auto &mcam = *MCamera.GetCamera<ManualCamera>();
      mcam.cameraTransform = &Owner->Transform;
      mcam.aspectRatio = newSize.x / newSize.y;
      Camera->Init();
    }
  }
}

// ----------------------------------------------------------------------------

void CameraComponent::OnUpdate(Events::EventMessage &)
{
  FixCameras();
  Camera->Update();
}

// ----------------------------------------------------------------------------

CameraComponentFactory::CameraComponentFactory()
  : allocator(sizeof(CameraComponent))
{
}

// ----------------------------------------------------------------------------

Component *CameraComponentFactory::CreateObject(void *memory, component_factory_data &data)
{
  MultiCam camera;
  auto target_name = data["target_name"].as_string();
  auto layer = static_cast<int>(map_fetch(data, "layer", 0).as_number());

  auto copyit = data.find("copy");
  if (copyit != data.end())
  {
    const auto &copyname = copyit->second.as_string();
    auto copyset = RenderGroup::Instance.GetSet(copyname);

    if (copyset)
    {
      auto icam = copyset->RenderCamera;
      auto multicam = reinterpret_cast<MultiCam *>(
        reinterpret_cast<byte *>(icam) +
        (reinterpret_cast<byte *>(&camera) -
          reinterpret_cast<byte *>(camera.Base))
      );

      return new(memory) CameraComponent(target_name, layer, multicam);
    }
  }

  auto type = map_fetch(data, "type", "ManualCamera").as_string();
  if (type == "HUDCamera")
  {
    camera.SetType<HUDCamera>();
  }
  else if (type == "LookAtCamera")
  {
    camera.SetType<LookAtCamera>();
  }
  else if (type == "ManualCamera")
  {
    camera.SetType<ManualCamera>();
  }
  else
  {
    throw string_exception{"Unknown Camera(Component) Type: " + type};
  }

  camera.Base->LoadFromData(data);
  camera.Base->Init();
  return new(memory) CameraComponent(target_name, layer, camera);
}

// ----------------------------------------------------------------------------