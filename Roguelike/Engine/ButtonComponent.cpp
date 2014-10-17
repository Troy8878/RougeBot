/*********************************
 * ButtonComponent.cpp
 * Leonardo Saikali
 * Created 2014/YOUR DATE HERE
 *********************************/

#include "Common.h"
#include "ButtonComponent.h"
#include "Button.h"
#include "Input.h"
#include "Camera.h"

// ----------------------------------------------------------------------------

ButtonComponentFactory ButtonComponent::factory;

// ----------------------------------------------------------------------------

ButtonComponent::ButtonComponent()
{
}

// ----------------------------------------------------------------------------

void ButtonComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);

  DEF_EVENT_ID(update);
  Owner->AddEvent(this, update, &ButtonComponent::OnUpdate);

  DEF_EVENT_ID(button_probe);
  Owner->AddEvent(this, button_probe, &ButtonComponent::OnProbe);
}

// ----------------------------------------------------------------------------

void ButtonComponent::OnUpdate(Events::EventMessage&)
{
}

// ----------------------------------------------------------------------------

void ButtonComponent::OnProbe(Events::EventMessage& e)
{
  auto& probe = *e.GetData<ButtonProbeEvent>();

  auto mousepos = Input::Instance.GetMouseState().position;
  
  // Button's Position = [transform] * [origin]
  auto position = Owner->Transform * DirectX::g_XMIdentityR3;
  // Normal = [transform] * [negative Z axis]
  auto normal = Owner->Transform * -DirectX::g_XMIdentityR2;

  float distance;

  auto projection = ScreenToPlane(mousepos, position, normal, 
                                  RenderTarget->RenderCamera, &distance);

  if (distance < 0)
    return;

  if (distance > probe.matchDistance && probe.matchDistance > 0)
    return;

  auto TrInverse = DirectX::XMMatrixInverse(nullptr, Owner->Transform);

  math::Vector2D curpos = TrInverse * projection;

  if (curpos.x > -Size.x / 2 && curpos.x < Size.x / 2 &&
      curpos.y > -Size.y / 2 && curpos.y < Size.y / 2)
  {
    probe.bestMatch = Owner;
    probe.matchDistance = distance;
  }
}

// ----------------------------------------------------------------------------

ButtonComponentFactory::ButtonComponentFactory()
  : allocator(sizeof(ButtonComponent))
{
}

// ----------------------------------------------------------------------------

Component *ButtonComponentFactory::CreateObject(
  void *memory, component_factory_data& data)
{
  auto set_name = data["render_target"].as_string();
  auto set = RenderGroup::Instance.GetSet(set_name);

  if (set == nullptr)
    throw string_exception("Render Target '" + set_name + 
                           "' could not be found while initializing SpriteComponent!");

  math::Vector2D size = {0,0};
  auto size_arr = data["size"].as_array_of<json::value::number_t>();
  if (size_arr.size() == 2)
    size = math::Vector2D { (float)size_arr[0], (float)size_arr[1] };

  auto *component = new (memory) ButtonComponent;

  component->RenderTarget = set;
  component->Size = size;

  return component;
}

// ----------------------------------------------------------------------------

mrb_value ButtonComponent::GetRubyWrapper()
{
  ONE_TIME_MESSAGE("[WARN] TODO: Implement ruby wrapper for Button");
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------
