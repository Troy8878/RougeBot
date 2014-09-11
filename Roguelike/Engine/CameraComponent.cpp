/*********************************
 * CameraComponent.cpp
 * Connor Hilarides
 * Created 2014/09/06
 *********************************/

#include "Common.h"
#include "CameraComponent.h"
#include "RenderSet.h"

// ----------------------------------------------------------------------------

CameraComponentFactory CameraComponent::factory;

// ----------------------------------------------------------------------------

CameraComponent::CameraComponent(const std::string& name, int layer, const MultiCam& camera)
  : _Name(name), _camera(camera), _cameraPtr(&_camera)
{
  RenderGroup::Instance.CreateSet(name, Camera, layer, false);
}

// ----------------------------------------------------------------------------

CameraComponent::CameraComponent(const std::string& name, int layer, MultiCam *copy)
  : _Name(name), _cameraPtr(copy)
{
  RenderGroup::Instance.CreateSet(name, Camera, layer, false);
}

// ----------------------------------------------------------------------------

CameraComponent::~CameraComponent()
{
  RenderGroup::Instance.RemoveSet(Name);
}

// ----------------------------------------------------------------------------

void CameraComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);

  static Events::EventId updateId("update");
  Owner->AddEvent(this, updateId, &CameraComponent::OnUpdate);
}

// ----------------------------------------------------------------------------

void CameraComponent::FixCameras()
{
  auto newSize = GetGame()->GameDevice->GetSize();
  if (prevSize != newSize)
  {
    prevSize = newSize;
      
    if (CameraType == typeid(HUDCamera))
    {
        auto& hcam = *MCamera.GetCamera<HUDCamera>();
        hcam.size.x = hcam.size.y * newSize.x / newSize.y;
        Camera->Init();
    }
    else if (CameraType == typeid(LookAtCamera))
    {
      auto& lcam = *MCamera.GetCamera<LookAtCamera>();
      lcam.aspectRatio = newSize.x / newSize.y;
      Camera->Init();
    }
  }
}

// ----------------------------------------------------------------------------

void CameraComponent::OnUpdate(Events::EventMessage&)
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

Component *CameraComponentFactory::CreateObject(void *memory, component_factory_data& data)
{
  MultiCam camera;
  auto target_name = data["target_name"];
  auto layer = std::stoi(map_fetch(data, "layer", "0"));

  auto copyit = data.find("copy");
  if (copyit != data.end())
  {
    auto copyset = RenderGroup::Instance.GetSet(copyit->second);

    auto icam = copyset->RenderCamera;
    (icam);
    //auto multicam = reinterpret_cast<MultiCam *>()

    return new (memory) CameraComponent(target_name, layer, nullptr);
  }
  
  auto type = data["type"];
  if (type == "HUDCamera")
  {
    camera.SetType<HUDCamera>();
  }
  else if (type == "LookAtCamera")
  {
    camera.SetType<LookAtCamera>();
  }
  else if (type == "Basic3DCamera")
  {
    //camera.SetType<Basic3DCamera>();
  }
  else
  {
    throw string_exception{"Unknown Camera(Component) Type: " + type};
  }

  camera.Base->LoadFromData(data);
  camera.Base->Init();
  return new (memory) CameraComponent(target_name, layer, camera);
}

// ----------------------------------------------------------------------------
