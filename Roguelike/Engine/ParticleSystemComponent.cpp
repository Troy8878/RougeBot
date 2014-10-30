/*********************************
 * ParticleSystemComponent.cpp
 * Enrique Rodriguez
 * Created 2014/09/30
 *********************************/

#include "Common.h"
#include "ParticleSystemComponent.h"
#include "StandardShapes.h"
#include "Shader.h"
#include <random>
#include <chrono>

// ----------------------------------------------------------------------------

ParticleSystemComponentFactory ParticleSystemComponent::factory;

static void mrb_particlesystem_free(mrb_state *, void *) {}
static mrb_data_type mrb_particlesystemcomp_data_type;

static void mrb_particlesystemcomponent_init(mrb_state *mrb, RClass *module, RClass *base);
static mrb_value mrb_particlesystemcomponent_new(mrb_state *mrb, ParticleSystemComponent *comp);
static float random(float min, float max);
static mrb_value mrb_particlesystem_get_scale(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_get_rotation(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_get_velocity(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_get_rotvel(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_set_scale(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_set_rotation(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_set_velocity(mrb_state *mrb, mrb_value self);
static mrb_value mrb_particlesystem_set_rotvel(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

ParticleSystemComponent::ParticleSystemComponent(size_t maxParticles, RenderSet *target, Ranges& scale,
                                                 Ranges& rotation, Ranges& velocity, Ranges& rotVel, float rate)
  : system(maxParticles), renderTarget(target)
{
  system.model = GetUnitSquare();
  system.shader = RegisteredShaders["Textured"];
  system.camera = renderTarget->RenderCamera;

  scaleRange = scale;
  rotationRange = rotation;
  velocityRange = velocity;
  rotVelRange = rotVel;
  ParticleRate = rate;
}

// ----------------------------------------------------------------------------

ParticleSystemComponent::~ParticleSystemComponent()
{
  if (renderTarget)
    renderTarget->RemoveDrawable(this);
}

// ----------------------------------------------------------------------------

void ParticleSystemComponent::Initialize(Entity *owner, const std::string& name)
{
  Component::Initialize(owner, name);

  DEF_EVENT_ID(update);
  Owner->AddEvent(this, update, &ParticleSystemComponent::OnUpdate);

  renderTarget->AddDrawable(this, system.shader);
}

// ----------------------------------------------------------------------------

void ParticleSystemComponent::OnUpdate(Events::EventMessage& e)
{
  float dt = (float) e.GetData<Events::UpdateEvent>()->gameTime.Dt;

  static double tb = 0;
  tb += dt * ParticleRate;
  while (tb > 0)
  {
    //randomize particle
    system.particleTransform.scaleRate = math::Vector{ random(scaleRange.Xmin, scaleRange.Xmax),
                                                       0,
                                                       random(scaleRange.Ymin, scaleRange.Ymax),
                                                       0 };
    system.particleTransform.rotationRate = math::Vector{ random(rotationRange.Xmin, rotationRange.Xmax),
                                                          0,
                                                          random(rotationRange.Ymin, rotationRange.Ymax),
                                                          0 };
    system.particleTransform.absoluteVelocity = math::Vector{ random(velocityRange.Xmin, velocityRange.Xmax),
                                                              0,
                                                              random(velocityRange.Ymin, velocityRange.Ymax),
                                                              0 };
    system.particleTransform.rotationalVelocity = math::Vector{ random(rotVelRange.Xmin, rotVelRange.Xmax),
                                                                0,
                                                                random(rotVelRange.Ymin, rotVelRange.Ymax),
                                                                0 };
    
    system.SpawnParticle(Owner->Transform, 1);
    --tb;
  }

  system.Update(dt);
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
  void *memory, component_factory_data& data)
{
  auto targetName = data["render_target"].as_string();
  auto target = RenderGroup::Instance.GetSet(targetName);

  Ranges scale, rotation, velocity, rotVel;
  float rate;

  auto it = data.find("scale");
  if (it != data.end())
    scale = ParseRanges(it->second);
  else
    scale = Ranges { 1, 1, 1, 1 };

  it = data.find("rotation");
  if (it != data.end())
    rotation = ParseRanges(it->second);
  else
    rotation = Ranges { 0, 1, 0, 1 };

  it = data.find("velocity");
  if (it != data.end())
    velocity = ParseRanges(it->second);
  else
    velocity = Ranges { 0, 1, 0, 1 };

  it = data.find("rotationVelocity");
  if (it != data.end())
    rotVel = ParseRanges(it->second);
  else
    rotVel = Ranges { 0, 0, 0, 0 };

  it = data.find("rate");
  if (it != data.end())
    rate = ParseFloat(it->second);
  else
    rate = 10;

  auto *comp = new (memory) ParticleSystemComponent( 1000, target, scale, rotation, velocity, rotVel, rate);

  return comp;
}

// ----------------------------------------------------------------------------

float ParticleSystemComponent::GetScale(int index)
{
  switch (index)
  {
  case 0:
    return scaleRange.Xmin;
  case 1:
    return scaleRange.Xmax;
  case 2:
    return scaleRange.Ymin;
  case 3:
    return scaleRange.Ymax;
  default:
    return 0;
  }
}

float ParticleSystemComponent::GetRotation(int index)
{
  switch (index)
  {
  case 0:
    return rotationRange.Xmin;
  case 1:
    return rotationRange.Xmax;
  case 2:
    return rotationRange.Ymin;
  case 3:
    return rotationRange.Ymax;
  default:
    return 0;
  }
}

float ParticleSystemComponent::GetVelocity(int index)
{
  switch (index)
  {
  case 0:
    return velocityRange.Xmin;
  case 1:
    return velocityRange.Xmax;
  case 2:
    return velocityRange.Ymin;
  case 3:
    return velocityRange.Ymax;
  default:
    return 0;
  }
}

float ParticleSystemComponent::GetRotationVelocity(int index)
{
  switch (index)
  {
  case 0:
    return rotVelRange.Xmin;
  case 1:
    return rotVelRange.Xmax;
  case 2:
    return rotVelRange.Ymin;
  case 3:
    return rotVelRange.Ymax;
  default:
    return 0;
  }
}

// ----------------------------------------------------------------------------

void ParticleSystemComponent::SetScale(float Xmin, float Xmax, float Ymin, float Ymax)
{
  scaleRange = Ranges{ Xmin, Xmax, Ymin, Ymax };
}

void ParticleSystemComponent::SetRotation(float Xmin, float Xmax, float Ymin, float Ymax)
{
  rotationRange = Ranges{ Xmin, Xmax, Ymin, Ymax };
}

void ParticleSystemComponent::SetVelocity(float Xmin, float Xmax, float Ymin, float Ymax)
{
  velocityRange = Ranges{ Xmin, Xmax, Ymin, Ymax };
}

void ParticleSystemComponent::SetRotationVelocity(float Xmin, float Xmax, float Ymin, float Ymax)
{
  rotVelRange = Ranges{ Xmin, Xmax, Ymin, Ymax };
}

// ----------------------------------------------------------------------------

Ranges ParticleSystemComponentFactory::ParseRanges(json::value jv)
{
  auto nums = jv.as_array_of<json::value::number_t>();
  while (nums.size() < 4)
    nums.push_back(0);

  Ranges ranges = { (float)nums[0], (float)nums[1], (float)nums[2], (float)nums[3] };

  return ranges;
}

float ParticleSystemComponentFactory::ParseFloat(json::value jv)
{
  return (float) jv.as_number();
}

// ----------------------------------------------------------------------------

static void mrb_particlesystemcomponent_init(mrb_state *mrb, RClass *module, RClass *base)
{
  mrb_particlesystemcomp_data_type.dfree = mrb_particlesystem_free;
  mrb_particlesystemcomp_data_type.struct_name = "ParticleSystemComponent";

  auto particlesyst = mrb_define_class_under(mrb, module, "ParticleSystemComponent", base);

  mrb_define_method(mrb, particlesyst, "get_scale", mrb_particlesystem_get_scale, ARGS_REQ(1));
  mrb_define_method(mrb, particlesyst, "get_rotation", mrb_particlesystem_get_rotation, ARGS_REQ(1));
  mrb_define_method(mrb, particlesyst, "get_velocity", mrb_particlesystem_get_velocity, ARGS_REQ(1));
  mrb_define_method(mrb, particlesyst, "get_rot_vel", mrb_particlesystem_get_rotvel, ARGS_REQ(1));

  mrb_define_method(mrb, particlesyst, "set_scale", mrb_particlesystem_set_scale, ARGS_REQ(4));
  mrb_define_method(mrb, particlesyst, "set_rotation", mrb_particlesystem_set_rotation, ARGS_REQ(4));
  mrb_define_method(mrb, particlesyst, "set_velocity", mrb_particlesystem_set_velocity, ARGS_REQ(4));
  mrb_define_method(mrb, particlesyst, "set_rot_vel", mrb_particlesystem_set_rotvel, ARGS_REQ(4));
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
           GetComponentRClass()));
  return mrb_particlesystemcomponent_new(*mrb_inst, this);
}

// ----------------------------------------------------------------------------

static float random(float min, float max)
{
  // obtain a seed from the system clock:
  unsigned seed = (unsigned) std::chrono::system_clock::now().time_since_epoch().count();

  std::default_random_engine generator(seed);
  std::uniform_real_distribution<float> distribution(min, max);

  return distribution(generator);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_particlesystem_get_scale(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = (ParticleSystemComponent *)mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type);

  mrb_int index;
  mrb_get_args(mrb, "i", &index);

  return mrb_float_value(mrb, particlesyst->GetScale((int) index));
}

static mrb_value mrb_particlesystem_get_rotation(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = (ParticleSystemComponent *)mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type);

  mrb_int index;
  mrb_get_args(mrb, "i", &index);

  return mrb_float_value(mrb, particlesyst->GetRotation((int) index));
}

static mrb_value mrb_particlesystem_get_velocity(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = (ParticleSystemComponent *)mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type);

  mrb_int index;
  mrb_get_args(mrb, "i", &index);

  return mrb_float_value(mrb, particlesyst->GetVelocity((int) index));
}

static mrb_value mrb_particlesystem_get_rotvel(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = (ParticleSystemComponent *)mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type);

  mrb_int index;
  mrb_get_args(mrb, "i", &index);

  return mrb_float_value(mrb, particlesyst->GetRotationVelocity((int) index));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_particlesystem_set_scale(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = (ParticleSystemComponent *)mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type);
  
  mrb_float Xmin, Xmax, Ymin, Ymax;
  mrb_get_args(mrb, "ffff",  &Xmin, &Xmax, &Ymin, &Ymax);

  particlesyst->SetScale((float)Xmin, (float)Xmax, (float)Ymin, (float)Ymax);
  
  return mrb_nil_value();
}

static mrb_value mrb_particlesystem_set_rotation(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = (ParticleSystemComponent *)mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type);

  mrb_float Xmin, Xmax, Ymin, Ymax;
  mrb_get_args(mrb, "ffff", &Xmin, &Xmax, &Ymin, &Ymax);

  particlesyst->SetRotation((float)Xmin, (float)Xmax, (float)Ymin, (float)Ymax);

  return mrb_nil_value();
}

static mrb_value mrb_particlesystem_set_velocity(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = (ParticleSystemComponent *)mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type);

  mrb_float Xmin, Xmax, Ymin, Ymax;
  mrb_get_args(mrb, "ffff", &Xmin, &Xmax, &Ymin, &Ymax);

  particlesyst->SetVelocity((float)Xmin, (float)Xmax, (float)Ymin, (float)Ymax);

  return mrb_nil_value();
}

static mrb_value mrb_particlesystem_set_rotvel(mrb_state *mrb, mrb_value self)
{
  auto particlesyst = (ParticleSystemComponent *)mrb_data_get_ptr(mrb, self, &mrb_particlesystemcomp_data_type);

  mrb_float Xmin, Xmax, Ymin, Ymax;
  mrb_get_args(mrb, "ffff", &Xmin, &Xmax, &Ymin, &Ymax);

  particlesyst->SetRotationVelocity((float)Xmin, (float)Xmax, (float)Ymin, (float)Ymax);

  return mrb_nil_value();
}
