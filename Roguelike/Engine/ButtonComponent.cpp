/*********************************
 * ButtonComponent.cpp
 * Leonardo Saikali
 * Created 2014/YOUR DATE HERE
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "ButtonComponent.h"
#include "Button.h"
#include "Input.h"
#include "Camera.h"
#include "RubyWrappers.h"
#include "SpriteComponent.h"

// ----------------------------------------------------------------------------

ButtonComponentFactory ButtonComponent::factory;

// ----------------------------------------------------------------------------

ButtonComponent::ButtonComponent()
{
}

// ----------------------------------------------------------------------------

void ButtonComponent::Initialize(Entity *owner, const std::string &name)
{
  Component::Initialize(owner, name);

  DEF_EVENT_ID(update);
  Owner->AddEvent(this, update, &ButtonComponent::OnUpdate);

  DEF_EVENT_ID(button_probe);
  Owner->AddEvent(this, button_probe, &ButtonComponent::OnProbe);

  Sprite = Owner->GetComponent<SpriteComponent>("SpriteComponent");
}

// ----------------------------------------------------------------------------

void ButtonComponent::OnUpdate(Events::EventMessage &)
{
}

// ----------------------------------------------------------------------------

void ButtonComponent::OnProbe(Events::EventMessage &e)
{
  if (Sprite && !Sprite->Visible)
    return;

  auto &probe = *e.GetData<ButtonProbeEvent>();

  auto mousepos = Input::Instance.GetMouseState().position;

  // Button's Position = [transform] * [origin]
  auto position = Owner->Transform * DirectX::g_XMIdentityR3;
  // Normal = [transform] * [negative Z axis]
  auto normal = Owner->Transform * -DirectX::g_XMIdentityR2;

  float distance;

#pragma region Calculate frame size

  using namespace DirectX;
  XMFLOAT3 frame[] =
    {
      {-Size.x / 2, Size.y / 2, 0},
      {Size.x / 2, Size.y / 2, 0},
      {Size.x / 2, -Size.y / 2, 0},
    };
  XMFLOAT3 oframe[ARRAYSIZE(frame)];
  XMVector3TransformNormalStream(oframe, ARRAY_STRIDE(oframe),
                                 frame, ARRAY_STRIDE(frame),
                                 ARRAYSIZE(frame),
                                 // Hang on a sec...
                                 // Don't I already account for that by inverting the transform?
                                 //Owner->Transform
                                 XMMatrixIdentity()
                                 );

  auto vbwidth = XMVector3Length(XMLoadFloat3(oframe + 0) - XMLoadFloat3(oframe + 1));
  auto vbheight = XMVector3Length(XMLoadFloat3(oframe + 1) - XMLoadFloat3(oframe + 2));

  auto bwidth = XMVectorGetX(vbwidth);
  auto bheight = XMVectorGetX(vbheight);

#pragma endregion

  auto projection = ScreenToPlane(mousepos, position, normal,
                                  RenderTarget->RenderCamera, &distance);

  if (distance < 0)
    return;

  if (distance > probe.matchDistance && probe.matchDistance > 0)
    return;

  auto TrInverse = XMMatrixInverse(nullptr, Owner->Transform);

  math::Vector2D curpos = TrInverse * projection;

  if (curpos.x > -bwidth / 2 && curpos.x < bwidth / 2 &&
    curpos.y > -bheight / 2 && curpos.y < bheight / 2)
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
  void *memory, component_factory_data &data)
{
  auto set_name = data["render_target"].as_string();
  auto set = RenderGroup::Instance.GetSet(set_name);

  if (set == nullptr)
    throw string_exception("Render Target '" + set_name +
      "' could not be found while initializing SpriteComponent!");

  math::Vector2D size = {1,1};
  auto size_arr = data["size"].as_array_of<json::value::number_t>();
  if (size_arr.size() == 2)
    size = math::Vector2D{static_cast<float>(size_arr[0]), static_cast<float>(size_arr[1])};

  auto *component = new(memory) ButtonComponent;

  component->RenderTarget = set;
  component->Size = size;

  return component;
}

// ----------------------------------------------------------------------------

mrb_data_type mrb_button_dt;

static void mrb_button_gem_init(mrb_state *mrb, RClass *cmod, RClass *cbase);
static mrb_value mrb_button_new(mrb_state *mrb, ButtonComponent *button);

static mrb_value mrb_button_size(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

mrb_value ButtonComponent::GetRubyWrapper()
{
  RUN_ONCE(mrb_button_gem_init(*mrb_inst, GetComponentRModule(), GetComponentRClass()));
  return mrb_button_new(*mrb_inst, this);
}

// ----------------------------------------------------------------------------

static RClass *button_c;

static void mrb_button_gem_init(mrb_state *mrb, RClass *cmod, RClass *cbase)
{
  mrb_button_dt.dfree = ruby::data_nop_delete;
  mrb_button_dt.struct_name = typeid(ButtonComponent).name();

  auto *button = button_c = mrb_define_class_under(mrb, cmod, "ButtonComponent", cbase);

  mrb_define_class_method(mrb, button, "new", mrb_nop, ARGS_ANY());
}

// ----------------------------------------------------------------------------

static mrb_value mrb_button_new(mrb_state *mrb, ButtonComponent *button)
{
  auto *data = mrb_data_object_alloc(mrb, button_c, button, &mrb_button_dt);
  auto obj = mrb_obj_value(data);

  return obj;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_button_size(mrb_state *mrb, mrb_value self)
{
  auto *button = static_cast<ButtonComponent *>(mrb_data_get_ptr(mrb, self, &mrb_button_dt));
  return ruby::wrap_memory_vector(&button->Size);
}

// ----------------------------------------------------------------------------