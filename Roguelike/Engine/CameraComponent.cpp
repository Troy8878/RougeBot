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
  : _Name(name), _camera(camera)
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
  if (CameraType == typeid(HUDCamera))
  {
    auto newSize = GetGame()->GameDevice->GetSize();
    if (prevSize != newSize)
    {
      prevSize = newSize;
      auto& hcam = *MCamera.GetCamera<HUDCamera>();
      hcam.size.x = hcam.size.y * newSize.x / newSize.y;
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
  auto target_name = data["target_name"];
  auto layer = std::stoi(map_fetch(data, "layer", "0"));
  MultiCam camera;
  
  auto type = data["type"];
  if (type == "HUDCamera")
  {
    camera.SetType<HUDCamera>();
    auto& hudCam = *camera.GetCamera<HUDCamera>();

    static auto default_pos = "[0,0,0,1]";
    auto jpos = json::value::parse(map_fetch(data, "position", default_pos));
    hudCam.position = math::Vector::VectorFromJson(jpos);

    static auto default_size = "[1,1]";
    auto jsize = json::value::parse(map_fetch(data, "size", default_size));
    hudCam.size = math::Vector::VectorFromJson(jsize).get();
  }
  else if (type == "LookAtCamera")
  {
    //camera.SetType<LookAtCamera>();
    //auto& laCam = *camera.GetCamera<LookAtCamera>();
    //(laCam);
  }
  else
  {
    throw string_exception{"Unknown Camera(Component) Type: " + type};
  }

  camera.GetCamera<ICamera>()->Init();
  return new (memory) CameraComponent(target_name, layer, camera);
}

// ----------------------------------------------------------------------------
