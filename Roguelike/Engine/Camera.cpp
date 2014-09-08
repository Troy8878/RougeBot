/*********************************
 * Camera.cpp
 * Connor Hilarides
 * Created 2014/09/06
 *********************************/

#include "Common.h"
#include "Camera.h"
#include "RenderSet.h"

// ----------------------------------------------------------------------------

static ruby::ruby_module mrb_camera_module();
static ruby::ruby_class mrb_camera_base();
static mrb_value mrb_cameras_get_camera(mrb_state *mrb, mrb_value);
static mrb_value mrb_cameras_camera_init(mrb_state *mrb, mrb_value self);

static ruby::ruby_class mrb_hudcamera_class();
static mrb_value mrb_hudcamera_init(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

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
  return hudclass.new_inst(mrb_inst->wrap_native_ptr(static_cast<ICamera *>(this)));
}

// ----------------------------------------------------------------------------

#pragma region Base Camera Ruby

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
