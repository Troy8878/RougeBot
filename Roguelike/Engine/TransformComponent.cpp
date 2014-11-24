/*********************************
 * TransformComponent.cpp
 * Jake Robsahm, Enrique Rodriguez
 * Created 2014/08/22
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "TransformComponent.h"

#include "mruby/data.h"
#include "mruby/class.h"
#include "mruby/variable.h"

#include "RubyWrappers.h"

// ----------------------------------------------------------------------------

TransformComponent::TransformComponent(
  const math::Vector &position, const math::Vector &rotation,
  const math::Vector &scale)
  : Position(position), Rotation(rotation),
    Scale(scale)
{
}

// ----------------------------------------------------------------------------

static void mrb_transform_free(mrb_state *, void *)
{
}

static mrb_data_type mrb_transformcomp_data_type;

// ----------------------------------------------------------------------------

void TransformComponent::Initialize(Entity *owner, const std::string &name)
{
  Component::Initialize(owner, name);

  DEF_EVENT_ID(draw);
  Owner->AddEvent(this, draw, &TransformComponent::OnUpdate);
}

// ----------------------------------------------------------------------------

void TransformComponent::OnUpdate(Events::EventMessage &)
{
  UpdateMatrix();
}

// ----------------------------------------------------------------------------

void TransformComponent::UpdateMatrix()
{
  if (this->Static)
    return;

  using namespace DirectX;

  // Fix rotations so they don't fall prey to large number rounding errors
  Rotation.x = fmod(Rotation.x, math::pi * 2);
  Rotation.y = fmod(Rotation.y, math::pi * 2);
  Rotation.z = fmod(Rotation.z, math::pi * 2);

  // Scale, Rotate, and Translate
  // This brings it into V^[parent space]
  auto mat =
    XMMatrixScalingFromVector(Scale.get()) *
    XMMatrixRotationRollPitchYawFromVector(Rotation.get()) *
    XMMatrixTranslationFromVector(Position.get());

  Owner->LocalTransform = mat;
}

// ----------------------------------------------------------------------------

void TransformComponent::_SetIsStatic(bool value)
{
  if (value == _static)
    return;

  _static = value;

  DEF_EVENT_ID(draw);
  if (_static)
  {
    UpdateMatrix();
    Owner->RemoveEvent(this, draw);
  }
  else
  {
    Owner->AddEvent(this, draw, &TransformComponent::OnUpdate);
  }
}

// ----------------------------------------------------------------------------

TransformComponentFactory TransformComponent::factory;

// ----------------------------------------------------------------------------

TransformComponentFactory::TransformComponentFactory()
  : allocator(sizeof(TransformComponent))
{
}

// ----------------------------------------------------------------------------

Component *TransformComponentFactory::CreateObject(
  void *memory, component_factory_data &data)
{
  math::Vector position, rotation, scale;

  auto it = data.find("position");
  if (it != data.end())
    position = ParseVector(it->second);
  else
    position = math::Vector{0, 0, 0, 1};

  it = data.find("rotation");
  if (it != data.end())
    rotation = ParseVector(it->second);
  else
    rotation = math::Vector{0, 0, 0, 0};

  it = data.find("scale");
  if (it != data.end())
    scale = ParseVector(it->second);
  else
    scale = math::Vector{1, 1, 1, 1};

  position.w = 1;
  rotation.w = 0;
  scale.w = 1;

  if (scale.z == 0)
    scale.z = 1;

  auto comp = new(memory) TransformComponent(position, rotation, scale);

  return comp;
}

// ----------------------------------------------------------------------------

math::Vector TransformComponentFactory::ParseVector(json::value jv)
{
  auto nums = jv.as_array_of<json::value::number_t>();
  while (nums.size() < 4)
    nums.push_back(0);

  return math::Vector
  {
    static_cast<float>(nums[0]),
    static_cast<float>(nums[1]),
    static_cast<float>(nums[2]),
    static_cast<float>(nums[3])
  };
}

// ----------------------------------------------------------------------------

static mrb_value rb_transform_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value tc_wrapper; // TransformComponent *
  mrb_get_args(mrb, "o", &tc_wrapper);

  ruby::save_native_ptr(mrb, self, mrb_ptr(tc_wrapper));

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static TransformComponent *get_rb_tc_wrapper(mrb_state *mrb, mrb_value self)
{
  //auto tc_wrapper = ruby::read_native_ptr<TransformComponent>(mrb, self);
  return static_cast<TransformComponent *>(mrb_data_get_ptr(mrb, self, &mrb_transformcomp_data_type));
}

static mrb_value rb_transform_position(mrb_state *mrb, mrb_value self)
{
  auto comp = get_rb_tc_wrapper(mrb, self);
  return ruby::wrap_memory_vector(&comp->Position);
}

static mrb_value rb_transform_position_set(mrb_state *mrb, mrb_value self)
{
  auto comp = get_rb_tc_wrapper(mrb, self);

  mrb_value value;
  mrb_get_args(mrb, "o", &value);
  auto vect = ruby::get_ruby_vector(value);

  comp->Position = vect;
  return value;
}

static mrb_value rb_transform_rotation(mrb_state *mrb, mrb_value self)
{
  auto comp = get_rb_tc_wrapper(mrb, self);
  return ruby::wrap_memory_vector(&comp->Rotation);
}

static mrb_value rb_transform_rotation_set(mrb_state *mrb, mrb_value self)
{
  auto comp = get_rb_tc_wrapper(mrb, self);

  mrb_value value;
  mrb_get_args(mrb, "o", &value);
  auto vect = ruby::get_ruby_vector(value);

  comp->Rotation = vect;
  return value;
}

static mrb_value rb_transform_scale(mrb_state *mrb, mrb_value self)
{
  auto comp = get_rb_tc_wrapper(mrb, self);
  return ruby::wrap_memory_vector(&comp->Scale);
}

static mrb_value rb_transform_scale_set(mrb_state *mrb, mrb_value self)
{
  auto comp = get_rb_tc_wrapper(mrb, self);

  mrb_value value;
  mrb_get_args(mrb, "o", &value);
  auto vect = ruby::get_ruby_vector(value);

  comp->Scale = vect;
  return value;
}

// ----------------------------------------------------------------------------

static mrb_value rb_transform_get_static(mrb_state *mrb, mrb_value self)
{
  auto comp = get_rb_tc_wrapper(mrb, self);
  return mrb_bool_value(comp->Static);
}

static mrb_value rb_transform_set_static(mrb_state *mrb, mrb_value self)
{
  mrb_bool newval;
  mrb_get_args(mrb, "b", &newval);

  auto comp = get_rb_tc_wrapper(mrb, self);
  comp->Static = !!newval;

  return mrb_bool_value(newval);
}

// ----------------------------------------------------------------------------

static void mrb_transformcomponent_init(mrb_state *mrb, RClass *module, RClass *base)
{
  mrb_transformcomp_data_type.dfree = mrb_transform_free;
  mrb_transformcomp_data_type.struct_name = "TransformComponent";

  auto transform = mrb_define_class_under(mrb, module, "TransformComponent", base);

  mrb_define_method(mrb, transform, "initialize", rb_transform_initialize, ARGS_REQ(1));

  mrb_define_method(mrb, transform, "position", rb_transform_position, ARGS_NONE());
  mrb_define_method(mrb, transform, "position=", rb_transform_position_set, ARGS_REQ(1));
  comp_add_property(mrb, transform, "position", "vector");

  mrb_define_method(mrb, transform, "rotation", rb_transform_rotation, ARGS_NONE());
  mrb_define_method(mrb, transform, "rotation=", rb_transform_rotation_set, ARGS_REQ(1));
  comp_add_property(mrb, transform, "rotation", "vector");

  mrb_define_method(mrb, transform, "scale", rb_transform_scale, ARGS_NONE());
  mrb_define_method(mrb, transform, "scale=", rb_transform_scale_set, ARGS_REQ(1));
  comp_add_property(mrb, transform, "scale", "vector");

  mrb_define_method(mrb, transform, "static", rb_transform_get_static, ARGS_NONE());
  mrb_define_method(mrb, transform, "static=", rb_transform_set_static, ARGS_REQ(1));
  comp_add_property(mrb, transform, "static", "bool", true);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_transformcomponent_new(mrb_state *mrb, TransformComponent *comp)
{
  auto rmod = mrb_module_get(mrb, "Components");
  auto rclass = mrb_class_get_under(mrb, rmod, "TransformComponent");

  auto obj = mrb_data_object_alloc(mrb, rclass, comp, &mrb_transformcomp_data_type);
  return mrb_obj_value(obj);
}

// ----------------------------------------------------------------------------

mrb_value TransformComponent::GetRubyWrapper()
{
  RUN_ONCE(mrb_transformcomponent_init(*mrb_inst, GetComponentRModule(), GetComponentRClass()));
  return mrb_transformcomponent_new(*mrb_inst, this);
}

// ----------------------------------------------------------------------------