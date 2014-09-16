/*********************************
 * TransformComponent.cpp
 * Jake Robsahm
 * Created 2014/08/22
 *********************************/

#include "Common.h"
#include "TransformComponent.h"

#include <sstream>
#include <regex>

#include "mruby/data.h"
#include "mruby/class.h"
#include "mruby/variable.h"

#include "RubyWrappers.h"

// ----------------------------------------------------------------------------

TransformComponent::TransformComponent(
  const math::Vector& position, const math::Vector& rotation, 
  const math::Vector& scale)
  : Position(position), Rotation(rotation),
    Scale(scale)
{
}

// ----------------------------------------------------------------------------

void TransformComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);

  static Events::EventId updateId("update");
  Owner->AddEvent(this, updateId, &TransformComponent::OnUpdate);
}

// ----------------------------------------------------------------------------

void TransformComponent::OnUpdate(Events::EventMessage&)
{
  UpdateMatrix();
}

// ----------------------------------------------------------------------------

void TransformComponent::UpdateMatrix()
{
  using namespace DirectX;

  // Fix rotations so they don't fall prey to large number rounding errors
  Rotation.x = fmod(Rotation.x, math::pi * 2);
  Rotation.y = fmod(Rotation.y, math::pi * 2);
  Rotation.z = fmod(Rotation.z, math::pi * 2);

  // Scale, Rotate, and Translate
  // This brings it into V^[parent space]
  auto mat = XMMatrixScalingFromVector(Scale.get()) *
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

  static Events::EventId updateId("update");
  if (_static)
  {
    UpdateMatrix();
    Owner->RemoveEvent(this, updateId);
  }
  else
  {
    Owner->AddEvent(this, updateId, &TransformComponent::OnUpdate);
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
  void *memory, component_factory_data& data)
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

  auto comp = new (memory) TransformComponent(position, rotation, scale);

  return comp;
}

// ----------------------------------------------------------------------------

math::Vector TransformComponentFactory::ParseVector(json::value jv)
{
  auto nums = jv.as_array_of<json::value::number_t>();
  while (nums.size() < 4)
    nums.push_back(0);

  return math::Vector{(float)nums[0], (float)nums[1], (float)nums[2], (float)nums[3]};
}

// ----------------------------------------------------------------------------

static mrb_value rb_transform_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value tc_wrapper; // TransformComponent *
  mrb_get_args(mrb, "o", &tc_wrapper);

  // ruby equiv:
  //  @comp_ptr = tc_wrapper
  static mrb_sym tc_wrapper_sym = mrb_intern_cstr(mrb, "comp_ptr");
  mrb_iv_set(mrb, self, tc_wrapper_sym, tc_wrapper);

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static TransformComponent *get_rb_tc_wrapper(mrb_state *mrb, mrb_value self)
{
  ruby::ruby_engine engine{mrb};

  static mrb_sym tc_wrapper_sym = mrb_intern_cstr(mrb, "comp_ptr");
  auto tc_wrapper = mrb_iv_get(mrb, self, tc_wrapper_sym);

  return (TransformComponent *) engine.unwrap_native_ptr(tc_wrapper);
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

mrb_value TransformComponent::GetRubyWrapper()
{
  THREAD_EXCLUSIVE_SCOPE;

  static bool init = false;
  static ruby::ruby_class comp_class;
  
  if (!init)
  {
    auto comp_mod = GetComponentRModule();
    auto comp_base = GetComponentRClass();
    comp_class = comp_mod.define_class("TransformComponent", comp_base);

    comp_class.define_method("initialize", rb_transform_initialize, ARGS_REQ(1));

    comp_class.define_method("position", rb_transform_position, ARGS_NONE());
    comp_class.define_method("position=", rb_transform_position_set, ARGS_REQ(1));
    
    comp_class.define_method("rotation", rb_transform_rotation, ARGS_NONE());
    comp_class.define_method("rotation=", rb_transform_rotation_set, ARGS_REQ(1));

    comp_class.define_method("scale", rb_transform_scale, ARGS_NONE());
    comp_class.define_method("scale=", rb_transform_scale_set, ARGS_REQ(1));

    comp_class.define_method("static", rb_transform_get_static, ARGS_NONE());
    comp_class.define_method("static=", rb_transform_set_static, ARGS_REQ(1));

    init = true;
  }

  auto compwrap = mrb_inst->wrap_native_ptr(this);
  return comp_class.new_inst(compwrap).silent_reset();
}

// ----------------------------------------------------------------------------
