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

// ----------------------------------------------------------------------------

TransformComponent::TransformComponent(
  const math::Vector& position, const math::Vector& rotation, 
  const math::Vector& scale)
  : _Position(position), _Rotation(rotation),
    _Scale(scale)
{
  UpdateMatrix();
}

// ----------------------------------------------------------------------------

void TransformComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);

  static Events::EventId updateId("update");
  Owner->AddEvent(this, updateId, &TransformComponent::OnUpdate);
}

// ----------------------------------------------------------------------------

void TransformComponent::OnUpdate(Events::EventMessage& e)
{
  auto& time = e.GetData<Events::UpdateEvent>()->gameTime;

  Rotation.y += (float) time.Dt;

  UpdateMatrix();
}

// ----------------------------------------------------------------------------

void TransformComponent::UpdateMatrix()
{
  using namespace DirectX;

  Matrix = XMMatrixScalingFromVector(Scale.get()) *
           XMMatrixRotationRollPitchYawFromVector(Rotation.get()) *
           XMMatrixTranslationFromVector(Position.get());
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

  auto comp = new (memory) TransformComponent(position, rotation, scale);

  return comp;
}

// ----------------------------------------------------------------------------

math::Vector TransformComponentFactory::ParseVector(const std::string& str)
{
  std::regex reg_num{"([-+])?([\\d]+)(\\.([\\d]+))?(e([+-])?([\\d]+))?"};
  std::smatch results;
  std::regex_search(str, results, reg_num);

  float nums[4] = {0,0,0,0};
  int i = 0;
  for (auto& match : results)
  {
    if (match.matched)
    {
      nums[i++] = std::stof(match.str());

      if (i == 4)
        break;
    }
  }

  return math::Vector{nums[0], nums[1], nums[2], nums[3]};
}

// ----------------------------------------------------------------------------

static mrb_value rb_transform_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value tc_wrapper;

  mrb_get_args(mrb, "o", &tc_wrapper);

  static mrb_sym tc_wrapper_sym = mrb_intern_cstr(mrb, "");
  mrb_iv_set(mrb, self, tc_wrapper_sym, tc_wrapper);

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

ruby::ruby_value TransformComponent::GetRubyWrapper()
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
  }

  auto compwrap = ruby::ruby_engine::global_engine->wrap_native_ptr(this);
  return comp_class.new_inst_argv(&compwrap, 1);
}

// ----------------------------------------------------------------------------
