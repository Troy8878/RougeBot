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

static mrb_value mrb_cameras_make(ruby_class &cls, ICamera *cam);
static ruby_module mrb_camera_module();
static ruby_class mrb_camera_base();
static mrb_value mrb_cameras_get_camera(mrb_state *mrb, mrb_value);
static mrb_value mrb_cameras_camera_init(mrb_state *mrb, mrb_value self);
static mrb_value mrb_cameras_camera_screen_to_world(mrb_state *mrb, mrb_value self);

static ruby_class mrb_hudcamera_class();
static mrb_value mrb_hudcamera_init(mrb_state *mrb, mrb_value self);
static mrb_value mrb_hudcamera_position(mrb_state *mrb, mrb_value self);
static mrb_value mrb_hudcamera_get_size(mrb_state *mrb, mrb_value self);
static mrb_value mrb_hudcamera_set_size(mrb_state *mrb, mrb_value self);

static ruby_class mrb_lookatcamera_class();
static mrb_value mrb_lookatcamera_init(mrb_state *mrb, mrb_value self);
static mrb_value mrb_lookatcamera_position(mrb_state *mrb, mrb_value self);
static mrb_value mrb_lookatcamera_position_set(mrb_state *mrb, mrb_value self);
static mrb_value mrb_lookatcamera_look_at(mrb_state *mrb, mrb_value self);
static mrb_value mrb_lookatcamera_look_at_set(mrb_state *mrb, mrb_value self);
static mrb_value mrb_lookatcamera_get_fov(mrb_state *mrb, mrb_value self);
static mrb_value mrb_lookatcamera_set_fov(mrb_state *mrb, mrb_value self);

static ruby_class mrb_manualcamera_class();
static mrb_value mrb_manualcamera_init(mrb_state *mrb, mrb_value self);
static mrb_value mrb_manualcamera_get_fov(mrb_state *mrb, mrb_value self);
static mrb_value mrb_manualcamera_set_fov(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

#pragma region ICamera Impls

void HUDCamera::LoadFromData(const component_factory_data &data)
{
  auto jpos = map_fetch(data, "position", json::value::parse("[0,0,0,1]"));
  auto jsize = map_fetch(data, "size", json::value::parse("[1,1]"));

  position = math::Vector::VectorFromJson(jpos);
  size = math::Vector::VectorFromJson(jsize).get();

  position.w = 1;
}

// ----------------------------------------------------------------------------

mrb_value HUDCamera::GetRubyWrapper()
{
  static auto hudclass = mrb_hudcamera_class();
  return mrb_cameras_make(hudclass, this);
}

// ----------------------------------------------------------------------------

void LookAtCamera::LoadFromData(const component_factory_data &data)
{
  static auto dpos = json::value::parse("[0,0,1,1]");
  static auto dlook = json::value::parse("[0,0,0,1]");

  auto jnear = map_fetch(data, "near", 0.01);
  auto jfar = map_fetch(data, "far", 100);
  auto jfov = map_fetch(data, "fov", 45);
  auto jpos = map_fetch(data, "position", dpos);
  auto jlook = map_fetch(data, "look_at", dlook);

  nearField = static_cast<float>(jnear.as_number());
  farField = static_cast<float>(jfar.as_number());
  fieldOfView = float(jfov.as_number() * math::pi / 180.0);
  position = math::Vector::VectorFromJson(jpos);
  lookAt = math::Vector::VectorFromJson(jlook);

  position.w = 1;
  lookAt.w = 1;
}

// ----------------------------------------------------------------------------

mrb_value LookAtCamera::GetRubyWrapper()
{
  static auto lookclass = mrb_lookatcamera_class();
  return mrb_cameras_make(lookclass, this);
}

// ----------------------------------------------------------------------------

void ManualCamera::LoadFromData(const component_factory_data &data)
{
  auto jnear = map_fetch(data, "near", 0.01);
  auto jfar = map_fetch(data, "far", 100);
  auto jfov = map_fetch(data, "fov", 45);

  nearField = static_cast<float>(jnear.as_number());
  farField = static_cast<float>(jfar.as_number());
  fieldOfView = float(jfov.as_number() * math::pi / 180.0);
}

// ----------------------------------------------------------------------------

mrb_value ManualCamera::GetRubyWrapper()
{
  static auto manualclass = mrb_manualcamera_class();
  return mrb_cameras_make(manualclass, this);
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Base Camera Ruby

static mrb_value mrb_cameras_make(ruby::ruby_class &cls, ICamera *cam)
{
  return cls.new_inst(mrb_inst->wrap_native_ptr(cam)).silent_reset();
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
    base.define_method("screen_to_world", mrb_cameras_camera_screen_to_world, ARGS_REQ(1));

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

// ----------------------------------------------------------------------------

static mrb_value mrb_cameras_camera_screen_to_world(mrb_state *mrb, mrb_value self)
{
  using namespace DirectX;

  mrb_value point;
  mrb_value plane_origin = mrb_nil_value();
  mrb_value plane_normal = mrb_nil_value();
  mrb_get_args(mrb, "o|oo", &point, &plane_origin, &plane_normal);

  auto cam = ruby::read_native_ptr<ICamera>(mrb, self);
  XMVECTOR point_v = ruby::get_ruby_vector(point);
  XMVECTOR origin_v = g_XMZero;
  XMVECTOR normal_v = g_XMIdentityR2;

  if (!mrb_nil_p(plane_origin))
    origin_v = ruby::get_ruby_vector(plane_origin);
  if (!mrb_nil_p(plane_normal))
    normal_v = ruby::get_ruby_vector(plane_normal);

  auto result = ScreenToPlane(point_v, origin_v, normal_v, cam);
  return ruby::create_new_vector(result);
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
    lookcamera.define_method("position=", mrb_lookatcamera_position_set, ARGS_REQ(1));
    lookcamera.define_method("look_at", mrb_lookatcamera_look_at, ARGS_NONE());
    lookcamera.define_method("look_at=", mrb_lookatcamera_look_at_set, ARGS_REQ(1));
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
  auto &lcam = *static_cast<LookAtCamera *>(icam);

  return wrap_memory_vector(&lcam.position);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_lookatcamera_position_set(mrb_state *mrb, mrb_value self)
{
  mrb_value vv;
  mrb_get_args(mrb, "o", &vv);

  auto v = *static_cast<math::Vector *>(mrb_data_get_ptr(mrb, vv, &mrb_vector_type));
  auto icam = read_native_ptr<ICamera>(mrb, self);
  auto &lcam = *static_cast<LookAtCamera *>(icam);

  lcam.position = v;

  return vv;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_lookatcamera_look_at(mrb_state *mrb, mrb_value self)
{
  auto icam = read_native_ptr<ICamera>(mrb, self);
  auto &lcam = *static_cast<LookAtCamera *>(icam);

  return wrap_memory_vector(&lcam.lookAt);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_lookatcamera_look_at_set(mrb_state *mrb, mrb_value self)
{
  mrb_value vv;
  mrb_get_args(mrb, "o", &vv);

  auto v = *static_cast<math::Vector *>(mrb_data_get_ptr(mrb, vv, &mrb_vector_type));
  auto icam = read_native_ptr<ICamera>(mrb, self);
  auto &lcam = *static_cast<LookAtCamera *>(icam);

  lcam.lookAt = v;

  return vv;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_lookatcamera_get_fov(mrb_state *mrb, mrb_value self)
{
  auto icam = read_native_ptr<ICamera>(mrb, self);
  auto &lcam = *static_cast<LookAtCamera *>(icam);

  return mrb_float_value(mrb, lcam.fieldOfView);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_lookatcamera_set_fov(mrb_state *mrb, mrb_value self)
{
  auto icam = read_native_ptr<ICamera>(mrb, self);
  auto &lcam = *static_cast<LookAtCamera *>(icam);

  mrb_float fov;
  mrb_get_args(mrb, "f", &fov);

  lcam.fieldOfView = static_cast<float>(fov);
  lcam.Init();
  lcam.Update();

  return mrb_float_value(mrb, fov);
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region ManualCamera Ruby

static ruby_class mrb_manualcamera_class()
{
  static auto module = mrb_camera_module();
  static auto base = mrb_camera_base();
  static ruby::ruby_class manualcamera = module.define_class("ManualCamera", base);
  static bool init = false;

  if (!init)
  {
    manualcamera.define_method("initialize", mrb_manualcamera_init, ARGS_REQ(1));

    init = true;
  }

  return manualcamera;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_manualcamera_init(mrb_state *mrb, mrb_value self)
{
  return mrb_cameras_camera_init(mrb, self);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_manualcamera_get_fov(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

static mrb_value mrb_manualcamera_set_fov(mrb_state *mrb, mrb_value self);

#pragma endregion

// ----------------------------------------------------------------------------

void Basic3DCamera::LoadFromData(const component_factory_data&)
{
}

// ----------------------------------------------------------------------------

mrb_value Basic3DCamera::GetRubyWrapper()
{
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------
