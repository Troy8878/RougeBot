/*********************************
 * PositionComponent.cpp
 * YOUR NAME HERE
 * Created 2014/YOUR DATE HERE
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "PositionComponent.h"
#include "Engine/TransformComponent.h"

// ----------------------------------------------------------------------------

PositionComponentFactory PositionComponent::factory;

// ----------------------------------------------------------------------------

PositionComponent::PositionComponent()
  : position(0, 0, 0, 1)
{
}

// ----------------------------------------------------------------------------

void PositionComponent::Initialize(Entity* owner, const std::string& name)
{
  Component::Initialize(owner, name);
  translation = &owner->TGetComponent(TransformComponent)->Position;

  Owner->AddEvent(this, mrb_intern_lit(*mrb_inst, "update"), &PositionComponent::Update);
  Jump(0);
}

// ----------------------------------------------------------------------------

void PositionComponent::Update(Events::EventMessage& e)
{
  auto dt = e.GetData<Events::UpdateEvent>()->gameTime.Dt;
  (this->*modeFunc)(static_cast<float>(dt));
}

// ----------------------------------------------------------------------------

void PositionComponent::Jump(float)
{
  translation->x = position.x;
  translation->y = position.y;
}

// ----------------------------------------------------------------------------

void PositionComponent::Linear(float dt)
{
  using namespace DirectX;
  XMVECTOR diff = XMVectorSetZ(position - *translation, 0);

  float amt = dt * MoveSpeed;
  if (XMVectorGetX(XMVector3Length(diff)) > amt)
  {
    diff = XMVector3Normalize(diff) * amt;
  }

  translation->x += XMVectorGetX(diff);
  translation->y += XMVectorGetY(diff);
}

// ----------------------------------------------------------------------------

void PositionComponent::Exponential(float dt)
{
  using namespace DirectX;
  XMVECTOR diff = XMVectorSetZ(position - *translation, 0);

  diff *= dt * 50 / MoveSpeed;

  translation->x += XMVectorGetX(diff);
  translation->y += XMVectorGetY(diff);
}

// ----------------------------------------------------------------------------

PositionComponentFactory::PositionComponentFactory()
  : allocator(sizeof(PositionComponent))
{
}

// ----------------------------------------------------------------------------

Component* PositionComponentFactory::CreateObject(
  void* memory, component_factory_data& data)
{
  auto* component = new(memory) PositionComponent;

  // Get the speed
  component->MoveSpeed = static_cast<float>(map_fetch(data, "move_speed", 5).as_number());

  // Get the mode
  auto &modeString = data["mode"].as_string();
  if (modeString == "jump")
    component->Mode = PositionComponent::MovementMode::Jump;
  if (modeString == "linear")
    component->Mode = PositionComponent::MovementMode::Linear;
  if (modeString == "exponential")
    component->Mode = PositionComponent::MovementMode::Exponential;

  // Get the initial position
  component->Position = JsonToColor(data["position"]);
  component->Position.z = 0;
  component->Position.w = 0;

  return component;
}

// ----------------------------------------------------------------------------

void PositionComponent::SetMode(MovementMode mode)
{
  this->mode = mode;
  switch (mode)
  {
    case MovementMode::Jump:
      modeFunc = &PositionComponent::Jump;
      break;
    case MovementMode::Linear:
      modeFunc = &PositionComponent::Linear;
      break;
    case MovementMode::Exponential:
      modeFunc = &PositionComponent::Exponential;
      break;
  }
}

// ----------------------------------------------------------------------------

mrb_data_type mrb_pos_comp_dt;
static void mrb_mruby_pos_comp_init(mrb_state *mrb, RClass *mod, RClass *base);
static mrb_value mrb_pos_comp_new(mrb_state *mrb, PositionComponent *component);
mrb_value mrb_pos_comp_pos(mrb_state *mrb, mrb_value self);
static mrb_value mrb_pos_comp_pos_set(mrb_state *mrb, mrb_value self);
static mrb_value mrb_pos_comp_jump(mrb_state *mrb, mrb_value self);
static mrb_value mrb_pos_comp_linear(mrb_state *mrb, mrb_value self);
static mrb_value mrb_pos_comp_exponential(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

mrb_value PositionComponent::GetRubyWrapper()
{
  RUN_ONCE(mrb_mruby_pos_comp_init(*mrb_inst, 
           GetComponentRModule(), GetComponentRClass()));

  return mrb_pos_comp_new(*mrb_inst, this);
}

// ----------------------------------------------------------------------------

static void mrb_mruby_pos_comp_init(mrb_state *mrb, RClass *mod, RClass *base)
{
  ruby::data_type_init<PositionComponent>(mrb_pos_comp_dt, ruby::data_nop_delete);

  auto *cls = mrb_define_class_under(mrb, mod, "PositionComponent", base);
  mrb_define_class_method(mrb, cls, "new", mrb_nop, ARGS_ANY());

  mrb_func_t move_speed_get = 
    ruby::data_getter_access_float<
      PositionComponent, &mrb_pos_comp_dt,
      float &, &PositionComponent::_PropGetMoveSpeed>;
  mrb_func_t move_speed_set = 
    ruby::data_setter_access_float<
      PositionComponent, &mrb_pos_comp_dt,
      const float &, &PositionComponent::_PropSetMoveSpeed>;

  mrb_define_method(mrb, cls, "move_speed", move_speed_get, ARGS_NONE());
  mrb_define_method(mrb, cls, "move_speed=", move_speed_set, ARGS_REQ(1));
  comp_add_property(mrb, cls, "move_speed", "float", true);

  mrb_define_method(mrb, cls, "position", mrb_pos_comp_pos, ARGS_NONE());
  mrb_define_method(mrb, cls, "position=", mrb_pos_comp_pos_set, ARGS_REQ(1));
  comp_add_property(mrb, cls, "position", "vector");

  mrb_define_method(mrb, cls, "jump", mrb_pos_comp_jump, ARGS_REQ(1));
  mrb_define_method(mrb, cls, "linear", mrb_pos_comp_linear, ARGS_REQ(1));
  mrb_define_method(mrb, cls, "exponential", mrb_pos_comp_exponential, ARGS_REQ(1));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_pos_comp_new(mrb_state *mrb, PositionComponent *component)
{
  static RClass *cls = mrb_class_get_under(mrb, Component::GetComponentRModule(), 
                                           "PositionComponent");
  auto data = mrb_data_object_alloc(mrb, cls, component, &mrb_pos_comp_dt);
  return mrb_obj_value(data);
}

// ----------------------------------------------------------------------------

mrb_value mrb_pos_comp_pos(mrb_state *mrb, mrb_value self)
{
  auto comp = ruby::data_get<PositionComponent>(mrb, self);
  return ruby::wrap_memory_vector(&comp->position);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_pos_comp_pos_set(mrb_state *mrb, mrb_value self)
{
  mrb_value mrb_vect;
  mrb_get_args(mrb, "o", &mrb_vect);

  auto comp = ruby::data_get<PositionComponent>(mrb, self);
  auto &vect = ruby::get_ruby_vector(mrb_vect);

  comp->Position = vect;

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_pos_comp_jump(mrb_state *mrb, mrb_value self)
{
  mrb_float dt;
  mrb_get_args(mrb, "f", &dt);

  auto comp = ruby::data_get<PositionComponent>(mrb, self);
  comp->Jump(static_cast<float>(dt));

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_pos_comp_linear(mrb_state *mrb, mrb_value self)
{
  mrb_float dt;
  mrb_get_args(mrb, "f", &dt);

  auto comp = ruby::data_get<PositionComponent>(mrb, self);
  comp->Linear(static_cast<float>(dt));

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_pos_comp_exponential(mrb_state *mrb, mrb_value self)
{
  mrb_float dt;
  mrb_get_args(mrb, "f", &dt);

  auto comp = ruby::data_get<PositionComponent>(mrb, self);
  comp->Exponential(static_cast<float>(dt));

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------