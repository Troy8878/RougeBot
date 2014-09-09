/*********************************
 * Camera.cpp
 * Connor Hilarides
 * Created 2014/09/06
 *********************************/

#include "Common.h"
#include "Camera.h"
#include "RenderSet.h"
#include "RubyWrappers.h"

// ----------------------------------------------------------------------------

using namespace ruby;

static mrb_value mrb_cameras_make(ruby_class& cls, ICamera *cam);
static ruby_module mrb_camera_module();
static ruby_class mrb_camera_base();
static mrb_value mrb_cameras_get_camera(mrb_state *mrb, mrb_value);
static mrb_value mrb_cameras_camera_init(mrb_state *mrb, mrb_value self);

static ruby_class mrb_hudcamera_class();
static mrb_value mrb_hudcamera_init(mrb_state *mrb, mrb_value self);
static mrb_value mrb_hudcamera_position(mrb_state *mrb, mrb_value self);
static mrb_value mrb_hudcamera_get_size(mrb_state *mrb, mrb_value self);
static mrb_value mrb_hudcamera_set_size(mrb_state *mrb, mrb_value self);

static ruby_class mrb_lookatcamera_class();
static mrb_value mrb_lookatcamera_init(mrb_state *mrb, mrb_value self);
static mrb_value mrb_lookatcamera_position(mrb_state *mrb, mrb_value self);
static mrb_value mrb_lookatcamera_look_at(mrb_state *mrb, mrb_value self);
static mrb_value mrb_lookatcamera_get_fov(mrb_state *mrb, mrb_value self);
static mrb_value mrb_lookatcamera_set_fov(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

#pragma region ICamera Impls

void HUDCamera::LoadFromData(const component_factory_data& data)
{
  auto jpos = json::value::parse(map_fetch(data, "position", "[0,0,0,1]"));
  auto jsize = json::value::parse(map_fetch(data, "size", "[1,1]"));

  position = math::Vector::VectorFromJson(jpos);
  size = math::Vector::VectorFromJson(jsize).get();
}

// ----------------------------------------------------------------------------

mrb_value HUDCamera::GetRubyWrapper()
{
  static auto hudclass = mrb_hudcamera_class();
  return mrb_cameras_make(hudclass, this);
}

// ----------------------------------------------------------------------------

void LookAtCamera::LoadFromData(const component_factory_data& data)
{
  auto jpos = json::value::parse(map_fetch(data, "position", "[0,0,1,1]"));
  auto jlook = json::value::parse(map_fetch(data, "look_at", "[0,0,0,1]"));
  auto jfov = json::value::parse(map_fetch(data, "fov", "45"));

  position = math::Vector::VectorFromJson(jpos);
  lookAt = math::Vector::VectorFromJson(jlook);
  fieldOfView = float(jfov.as_number() * math::pi / 180.0);
}

// ----------------------------------------------------------------------------

mrb_value LookAtCamera::GetRubyWrapper()
{
  static auto lookclass = mrb_lookatcamera_class();
  return mrb_cameras_make(lookclass, this);
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Base Camera Ruby

static mrb_value mrb_cameras_make(ruby::ruby_class& cls, ICamera *cam)
{
  return cls.new_inst(mrb_inst->wrap_native_ptr(static_cast<ICamera *>(cam)));
}

// ----------------------------------------------------------------------------

static ruby::ruby_module mrb_camera_module()
{
  static ruby::ruby_module module = mrb_inst->define_module("Cameras");
  static bool init = false;

  if (!init)
  {
    module.define_module_method("find_camera", mrb_cameras_get_camera, ARGS_REQ(1));

    init = true;
  }

  return module;
}

// ----------------------------------------------------------------------------

static ruby::ruby_class mrb_camera_base()
{
  static ruby::ruby_class base = mrb_camera_module().define_class("Camera");
  static bool init = false;

  if (!init)
  {
    base.define_method("initialize", mrb_cameras_camera_init, ARGS_REQ(1));

    init = true;
  }
  
  return base;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_cameras_get_camera(mrb_state *mrb, mrb_value)
{
  const char *name;
  mrb_get_args(mrb, "z", &name);

  auto set = RenderGroup::Instance.GetSet(name);
  return set->RenderCamera->GetRubyWrapper();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_cameras_camera_init(mrb_state *mrb, mrb_value self)
{
  mrb_value native_ptr;
  mrb_get_args(mrb, "o", &native_ptr);

  ruby::save_native_ptr(mrb, self, mrb_cptr(native_ptr));
  
  return mrb_nil_value();
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region HUDCamera Ruby

static ruby::ruby_class mrb_hudcamera_class()
{
  static auto module = mrb_camera_module();
  static auto base = mrb_camera_base();
  static ruby::ruby_class hudcamera = module.define_class("HUDCamera", base);
  static bool init = false;

  if (!init)
  {
    hudcamera.define_method("initialize", mrb_hudcamera_init, ARGS_REQ(1));

    init = true;
  }

  return hudcamera;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_hudcamera_init(mrb_state *mrb, mrb_value self)
{
  return mrb_cameras_camera_init(mrb, self);
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region LookAtCamera Ruby

static ruby::ruby_class mrb_lookatcamera_class()
{
  static auto module = mrb_camera_module();
  static auto base = mrb_camera_base();
  static ruby_class lookcamera = module.define_class("LookAtCamera", base);
  static bool init = false;

  if (!init)
  {
    lookcamera.define_method("initialize", mrb_lookatcamera_init, ARGS_REQ(1));
    lookcamera.define_method("position", mrb_lookatcamera_position, ARGS_NONE());
    lookcamera.define_method("look_at", mrb_lookatcamera_look_at, ARGS_NONE());
    lookcamera.define_method("fov", mrb_lookatcamera_get_fov, ARGS_NONE());
    lookcamera.define_method("fov=", mrb_lookatcamera_set_fov, ARGS_REQ(1));

    init = true;
  }

  return lookcamera;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_lookatcamera_init(mrb_state *mrb, mrb_value self)
{
  return mrb_cameras_camera_init(mrb, self);
}

static mrb_value mrb_lookatcamera_position(mrb_state *mrb, mrb_value self)
{
  auto icam = read_native_ptr<ICamera>(mrb, self);
  auto& lcam = *static_cast<LookAtCamera *>(icam);

  return wrap_memory_vector(&lcam.position);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_lookatcamera_look_at(mrb_state *mrb, mrb_value self)
{
  auto icam = read_native_ptr<ICamera>(mrb, self);
  auto& lcam = *static_cast<LookAtCamera *>(icam);

  return wrap_memory_vector(&lcam.lookAt);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_lookatcamera_get_fov(mrb_state *mrb, mrb_value self)
{
  auto icam = read_native_ptr<ICamera>(mrb, self);
  auto& lcam = *static_cast<LookAtCamera *>(icam);

  return mrb_float_value(mrb, lcam.fieldOfView);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_lookatcamera_set_fov(mrb_state *mrb, mrb_value self)
{
  auto icam = read_native_ptr<ICamera>(mrb, self);
  auto& lcam = *static_cast<LookAtCamera *>(icam);

  mrb_float fov;
  mrb_get_args(mrb, "f", &fov);

  return mrb_float_value(mrb, lcam.fieldOfView = (float)fov);
}

#pragma endregion

// ----------------------------------------------------------------------------
