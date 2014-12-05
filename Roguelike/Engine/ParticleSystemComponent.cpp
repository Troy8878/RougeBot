/*********************************
 * ParticleSystemComponent.cpp
 * Enrique Rodriguez
 * Created 2014/09/30
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "ParticleSystemComponent.h"
#include "StandardShapes.h"
#include "Shader.h"
#include <random>
#include <chrono>
#include "RubyWrappers.h"

// ----------------------------------------------------------------------------

ParticleSystemComponentFactory ParticleSystemComponent::factory;

static void mrb_particlesystem_free(mrb_state *, void *)
{
}

static mrb_data_type mrb_particlesystemcomp_data_type;

template <typename Gen>
static float random(Gen &generator, float min, float max);

static void mrb_particlesystemcomponent_init(mrb_state *mrb, RClass *module, RClass *base);
static mrb_value mrb_particlesystemcomponent_new(mrb_state *mrb, ParticleSystemComponent *comp);
static mrb_value mrb_particlesystem_initialize(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_createParticle(mrb_state *mrb, mrb_value self);

static mrb_value mrb_particlesystem_get_scale(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_get_rotation(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_get_velocity(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_get_rotvel(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_get_particlerate(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_get_fadetime(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_get_active(mrb_state *mrb, mrb_value self);

static mrb_value mrb_particlesystem_set_scale(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_set_rotation(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_set_velocity(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_set_rotvel(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_set_particlerate(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_set_fadetime(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_set_active(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

ParticleSystemComponent::ParticleSystemComponent(size_t maxParticles, RenderSet *target,
  const math::Vector &scale, const math::Vector &rotation, const math::Vector &velocity,
  const math::Vector &rotVel, float rate, float fade, bool _active = true)
  : system(maxParticles), renderTarget(target), scaleRange(scale),
  rotationRange(rotation), velocityRange(velocity), rotVelRange(rotVel),
  particleRate(rate), fadeTime(fade), active(_active)
{
  system.model = GetUnitSquare();
  system.shader = RegisteredShaders["Textured"];
  system.camera = renderTarget->RenderCamera;
}

// ----------------------------------------------------------------------------

ParticleSystemComponent::~ParticleSystemComponent()
{
  if (renderTarget)
    renderTarget->RemoveDrawable(this);
}

// ----------------------------------------------------------------------------

void ParticleSystemComponent::Initialize(Entity *owner, const std::string &name)
{
  Component::Initialize(owner, name);

  DEF_EVENT_ID(update);
  Owner->AddEvent(this, update, &ParticleSystemComponent::OnUpdate);

  renderTarget->AddDrawable(this, system.shader);
}

// ----------------------------------------------------------------------------

void ParticleSystemComponent::OnUpdate(Events::EventMessage &e)
{
  float dt = (float) e.GetData<Events::UpdateEvent>()->gameTime.Dt;

  static std::random_device generator;

  static double tb = 0;
  if (active)
  {
    tb += particleRate * dt;
  }
    
  while (tb > 0)
  {
    //randomize particle
    system.particleTransform.scaleRate = math::Vector{ random(generator, scaleRange.x, scaleRange.y), random(generator, scaleRange.z, scaleRange.w), 0, 0 };
    system.particleTransform.rotationRate = math::Vector{ 0, 0, random(generator, rotationRange.x, rotationRange.y), 0 };
    system.particleTransform.absoluteVelocity = math::Vector{ random(generator, velocityRange.x, velocityRange.y), random(generator, velocityRange.z, velocityRange.w), 0, 0 };
    system.particleTransform.rotationalVelocity = math::Vector{ random(generator, rotVelRange.z, rotVelRange.w), 0, random(generator, rotVelRange.x, rotVelRange.y), 0 };
    system.particleTransform.fadeTime = fadeTime;

    system.SpawnParticle(Owner->Transform, 1);
    --tb;
  }

  system.Update(dt);
}

void ParticleSystemComponent::OnCall()
{
  static std::random_device generator;

  //randomize particle
  system.particleTransform.scaleRate = math::Vector{ random(generator, scaleRange.x, scaleRange.y), random(generator, scaleRange.z, scaleRange.w), 0, 0 };
  system.particleTransform.rotationRate = math::Vector{ 0, 0, random(generator, rotationRange.x, rotationRange.y), 0 };
  system.particleTransform.absoluteVelocity = math::Vector{ random(generator, velocityRange.x, velocityRange.y), random(generator, velocityRange.z, velocityRange.w), 0, 0 };
  system.particleTransform.rotationalVelocity = math::Vector{ random(generator, rotVelRange.z, rotVelRange.w), 0, random(generator, rotVelRange.x, rotVelRange.y), 0 };
  system.particleTransform.fadeTime = fadeTime;

  system.SpawnParticle(Owner->Transform, 1);
}

// ----------------------------------------------------------------------------

void ParticleSystemComponent::Draw()
{
  system.Draw();
}

// ----------------------------------------------------------------------------

Model *ParticleSystemComponent::GetUnitSquare()
{
  static Model *square = nullptr;

  if (!square)
    square = Shapes::MakeRectangle(GetGame()->GameDevice->Device, {0.1f, 0.1f});

  return square;
}

// ----------------------------------------------------------------------------

ParticleSystemComponentFactory::ParticleSystemComponentFactory()
  : allocator(sizeof(ParticleSystemComponent))
{
}

// ----------------------------------------------------------------------------

Component *ParticleSystemComponentFactory::CreateObject(
  void *memory, component_factory_data &data)
{
  auto targetName = data["render_target"].as_string();
  auto target = RenderGroup::Instance.GetSet(targetName);

  math::Vector scale, rotation, velocity, rotVel;
  float rate, fade;
  bool active;

  auto it = data.find("scaleRange");
  if (it != data.end())
    scale = ParseVector(it->second);
  else
    scale = math::Vector{1, 1, 1, 1};

  it = data.find("rotationRange");
  if (it != data.end())
    rotation = ParseVector(it->second);
  else
    rotation = math::Vector{-3, 3, 0, 0};

  it = data.find("velocityRange");
  if (it != data.end())
    velocity = ParseVector(it->second);
  else
    velocity = math::Vector{-1, 1, -1, 1};

  it = data.find("rotVelRange");
  if (it != data.end())
    rotVel = ParseVector(it->second);
  else
    rotVel = math::Vector{0, 0, 0, 0};

  it = data.find("particleRate");
  if (it != data.end())
    rate = ParseFloat(it->second);
  else
    rate = 10;

  it = data.find("fadeTime");
  if (it != data.end())
    fade = ParseFloat(it->second);
  else
    fade = 0.5;

  it = data.find("active");
  if (it != data.end())
    active = ParseBool(it->second);
  else
    active = true;

  auto *comp = new(memory)ParticleSystemComponent(1000, target, scale, rotation, velocity, rotVel, rate, fade, active);

  it = data.find("texture");
  if (it != data.end())
    comp->system.texture = TextureManager::Instance.LoadTexture(it->second.as_string());

  return comp;
}

// ----------------------------------------------------------------------------

math::Vector ParticleSystemComponentFactory::ParseVector(json::value jv)
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

float ParticleSystemComponentFactory::ParseFloat(json::value jv)
{
  return static_cast<float>(jv.as_number());
}

bool ParticleSystemComponentFactory::ParseBool(json::value jv)
{
  return static_cast<bool>(jv.as_bool());
}

// ----------------------------------------------------------------------------

float ParticleSystemComponent::GetScale(int index)
{
  switch (index)
  {
  case 0:
    return scaleRange.x;
  case 1:
    return scaleRange.y;
  case 2:
    return scaleRange.z;
  case 3:
    return scaleRange.w;
  default:
    return 0;
  }
}

float ParticleSystemComponent::GetRotation(int index)
{
  switch (index)
  {
  case 0:
    return rotationRange.x;
  case 1:
    return rotationRange.y;
  case 2:
    return rotationRange.z;
  case 3:
    return rotationRange.w;
  default:
    return 0;
  }
}

float ParticleSystemComponent::GetVelocity(int index)
{
  switch (index)
  {
  case 0:
    return velocityRange.x;
  case 1:
    return velocityRange.y;
  case 2:
    return velocityRange.z;
  case 3:
    return velocityRange.w;
  default:
    return 0;
  }
}

float ParticleSystemComponent::GetRotationVelocity(int index)
{
  switch (index)
  {
  case 0:
    return rotVelRange.x;
  case 1:
    return rotVelRange.y;
  case 2:
    return rotVelRange.z;
  case 3:
    return rotVelRange.w;
  default:
    return 0;
  }
}

float ParticleSystemComponent::GetParticleRate()
{
  return particleRate;
}

float ParticleSystemComponent::GetFadeTime()
{
  return fadeTime;
}

bool ParticleSystemComponent::GetActive()
{
  return active;
}

// ----------------------------------------------------------------------------

void ParticleSystemComponent::SetScale(math::Vector ranges)
{
  scaleRange = ranges;
}

void ParticleSystemComponent::SetRotation(math::Vector ranges)
{
  rotationRange = ranges;
}

void ParticleSystemComponent::SetVelocity(math::Vector ranges)
{
  velocityRange = ranges;
}

void ParticleSystemComponent::SetRotationVelocity(math::Vector ranges)
{
  rotVelRange = ranges;
}

void ParticleSystemComponent::SetParticleRate(float rate)
{
  particleRate = rate;
}

void ParticleSystemComponent::SetFadeTime(float time)
{
  fadeTime = time;
}

void ParticleSystemComponent::SetActive(bool _active)
{
  active = _active;
}

// ----------------------------------------------------------------------------

static void mrb_particlesystemcomponent_init(mrb_state *mrb, RClass *module, RClass *base)
{
  mrb_particlesystemcomp_data_type.dfree = mrb_particlesystem_free;
  mrb_particlesystemcomp_data_type.struct_name = "ParticleSystemComponent";

  auto particlesyst = mrb_define_class_under(mrb, module, "ParticleSystemComponent", base);

  mrb_define_method(mrb, particlesyst, "initialize", mrb_particlesystem_initialize, ARGS_REQ(1));
  mrb_define_method(mrb, particlesyst, "createParticle", mrb_particlesystem_createParticle, ARGS_NONE());

  mrb_define_method(mrb, particlesyst, "scaleRange", mrb_particlesystem_get_scale, ARGS_NONE());
  mrb_define_method(mrb, particlesyst, "rotationRange", mrb_particlesystem_get_rotation, ARGS_NONE());
  mrb_define_method(mrb, particlesyst, "velocityRange", mrb_particlesystem_get_velocity, ARGS_NONE());
  mrb_define_method(mrb, particlesyst, "rotVelRange", mrb_particlesystem_get_rotvel, ARGS_NONE());
  mrb_define_method(mrb, particlesyst, "particleRate", mrb_particlesystem_get_particlerate, ARGS_NONE());
  mrb_define_method(mrb, particlesyst, "fadeTime", mrb_particlesystem_get_fadetime, ARGS_NONE());
  mrb_define_method(mrb, particlesyst, "active", mrb_particlesystem_get_active, ARGS_NONE());

  mrb_define_method(mrb, particlesyst, "scaleRange=", mrb_particlesystem_set_scale, ARGS_REQ(1));
  mrb_define_method(mrb, particlesyst, "rotationRange=", mrb_particlesystem_set_rotation, ARGS_REQ(1));
  mrb_define_method(mrb, particlesyst, "velocityRange=", mrb_particlesystem_set_velocity, ARGS_REQ(1));
  mrb_define_method(mrb, particlesyst, "rotVelRange=", mrb_particlesystem_set_rotvel, ARGS_REQ(1));
  mrb_define_method(mrb, particlesyst, "particleRate=", mrb_particlesystem_set_particlerate, ARGS_REQ(1));
  mrb_define_method(mrb, particlesyst, "fadeTime=", mrb_particlesystem_set_fadetime, ARGS_REQ(1));
  mrb_define_method(mrb, particlesyst, "active=", mrb_particlesystem_set_active, ARGS_REQ(1));

  comp_add_property(mrb, particlesyst, "scaleRange", "vector");
  comp_add_property(mrb, particlesyst, "rotationRange", "vector");
  comp_add_property(mrb, particlesyst, "velocityRange", "vector");
  comp_add_property(mrb, particlesyst, "rotVelRange", "vector");
  comp_add_property(mrb, particlesyst, "particleRate", "float");
  comp_add_property(mrb, particlesyst, "fadeTime", "float");
  comp_add_property(mrb, particlesyst, "active", "bool");
}

// ----------------------------------------------------------------------------

static mrb_value mrb_particlesystemcomponent_new(mrb_state *mrb, ParticleSystemComponent *comp)
{
  auto rmod = mrb_module_get(mrb, "Components");
  auto rclass = mrb_class_get_under(mrb, rmod, "ParticleSystemComponent");

  auto obj = mrb_data_object_alloc(mrb, rclass, comp, &mrb_particlesystemcomp_data_type);
  return mrb_obj_value(obj);
}

// ----------------------------------------------------------------------------

mrb_value ParticleSystemComponent::GetRubyWrapper()
{
  RUN_ONCE(mrb_particlesystemcomponent_init(*mrb_inst,
                                            GetComponentRModule(),
                                            GetComponentRClass()
                                           )
          );
  return mrb_particlesystemcomponent_new(*mrb_inst, this);
}

// ----------------------------------------------------------------------------

template <typename Gen>
static float random(Gen &generator, float min, float max)
{
  std::uniform_real_distribution<float> distribution(min, max);
  return distribution(generator);
}

static mrb_value mrb_particlesystem_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value psc_wrapper;
  mrb_get_args(mrb, "o", &psc_wrapper);

  ruby::save_native_ptr(mrb, self, mrb_ptr(psc_wrapper));

  return mrb_nil_value();
}

static mrb_value mrb_particlesystem_createParticle(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = static_cast<ParticleSystemComponent *>(mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type));
  particlesyst->OnCall();
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_particlesystem_get_scale(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = static_cast<ParticleSystemComponent *>(mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type));
  return ruby::wrap_memory_vector(&particlesyst->scaleRange);
}

static mrb_value mrb_particlesystem_get_rotation(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = static_cast<ParticleSystemComponent *>(mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type));
  return ruby::wrap_memory_vector(&particlesyst->rotationRange);
}

static mrb_value mrb_particlesystem_get_velocity(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = static_cast<ParticleSystemComponent *>(mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type));
  return ruby::wrap_memory_vector(&particlesyst->velocityRange);
}

static mrb_value mrb_particlesystem_get_rotvel(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = static_cast<ParticleSystemComponent *>(mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type));
  return ruby::wrap_memory_vector(&particlesyst->rotVelRange);
}

static mrb_value mrb_particlesystem_get_particlerate(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = static_cast<ParticleSystemComponent *>(mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type));
  return mrb_float_value(mrb, particlesyst->particleRate);
}

static mrb_value mrb_particlesystem_get_fadetime(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = static_cast<ParticleSystemComponent *>(mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type));
  return mrb_float_value(mrb, particlesyst->fadeTime);
}

static mrb_value mrb_particlesystem_get_active(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = static_cast<ParticleSystemComponent *>(mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type));
  return mrb_bool_value(particlesyst->active);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_particlesystem_set_scale(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = static_cast<ParticleSystemComponent *>(mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type));

  mrb_value value;
  mrb_get_args(mrb, "o", &value);
  auto vect = ruby::get_ruby_vector(value);

  particlesyst->scaleRange = vect;
  return value;
}

static mrb_value mrb_particlesystem_set_rotation(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = static_cast<ParticleSystemComponent *>(mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type));

  mrb_value value;
  mrb_get_args(mrb, "o", &value);
  auto vect = ruby::get_ruby_vector(value);

  particlesyst->rotationRange = vect;
  return value;
}

static mrb_value mrb_particlesystem_set_velocity(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = static_cast<ParticleSystemComponent *>(mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type));

  mrb_value value;
  mrb_get_args(mrb, "o", &value);
  auto vect = ruby::get_ruby_vector(value);

  particlesyst->velocityRange = vect;
  return value;
}

static mrb_value mrb_particlesystem_set_rotvel(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = static_cast<ParticleSystemComponent *>(mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type));

  mrb_value value;
  mrb_get_args(mrb, "o", &value);
  auto vect = ruby::get_ruby_vector(value);

  particlesyst->rotVelRange = vect;
  return value;
}

static mrb_value mrb_particlesystem_set_particlerate(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = static_cast<ParticleSystemComponent *>(mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type));

  mrb_float value;
  mrb_get_args(mrb, "f", &value);

  particlesyst->particleRate = static_cast<float>(value);
  return mrb_float_value(mrb, value);
}

static mrb_value mrb_particlesystem_set_fadetime(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = static_cast<ParticleSystemComponent *>(mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type));

  mrb_float value;
  mrb_get_args(mrb, "f", &value);

  particlesyst->fadeTime = static_cast<float>(value);
  return mrb_float_value(mrb, value);
}

static mrb_value mrb_particlesystem_set_active(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = static_cast<ParticleSystemComponent *>(mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type));

  mrb_bool value;
  mrb_get_args(mrb, "b", &value);

  particlesyst->active = !!value;
  return mrb_bool_value(value);
}
