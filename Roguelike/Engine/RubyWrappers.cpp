/*********************************
 * RubyWrappers.cpp
 * Connor Hilarides
 * Created 2014/08/29
 *********************************/

#pragma once

#include "Common.h"
#include "RubyInterop.h"
#include "RubyWrappers.h"
#include "Game.h"

#include "mruby/string.h"
#include "mruby/data.h"
#include "mruby/class.h"
#include "mruby/compile.h"
#include "mruby/error.h"
#include "mruby/array.h"
#include "mruby/compile.h"
#include "mruby/variable.h"

// ----------------------------------------------------------------------------

#pragma region MemoryVector

namespace memvect 
{
#define MEMVECT_BASICS \
  using namespace ruby;\
  ruby_engine engine{mrb};\
  static auto ptr_sym = mrb_intern_cstr(mrb, "vect_ptr");\
  auto ptr_v = mrb_iv_get(mrb, self, ptr_sym);\
  math::Vector *vect = (math::Vector *) engine.unwrap_native_ptr(ptr_v)

#define MEMVECT_GETFUN(v) \
  static mrb_value get_ ## v (mrb_state *mrb, mrb_value self) \
  { \
    MEMVECT_BASICS; \
    return mrb_float_value(mrb, vect->v); \
  }

#define MEMVECT_SETFUN(v) \
  static mrb_value set_ ## v (mrb_state *mrb, mrb_value self) \
  { \
    MEMVECT_BASICS; \
    mrb_float newv; \
    mrb_get_args(mrb, "f", &newv); \
    return mrb_float_value(mrb, vect->v = (float) newv); \
  }

  MEMVECT_GETFUN(x);
  MEMVECT_SETFUN(x);

  MEMVECT_GETFUN(y);
  MEMVECT_SETFUN(y);

  MEMVECT_GETFUN(z);
  MEMVECT_SETFUN(z);

  MEMVECT_GETFUN(w);
  MEMVECT_SETFUN(w);

#undef MEMVECT_BASICS
#undef MEMVECT_GETFUN
#undef MEMVECT_SETFUN

  static mrb_value init(mrb_state *mrb, mrb_value self)
  {
    mrb_value vect_ptr_v;
    mrb_get_args(mrb, "o", &vect_ptr_v);

    static auto ptr_sym = mrb_intern_cstr(mrb, "vect_ptr");
    mrb_iv_set(mrb, self, ptr_sym, vect_ptr_v);

    return mrb_nil_value();
  }
}

static ruby::ruby_class memvect2dclass;

static ruby::ruby_class create_memory_vector_class()
{
  auto rclass2D = mrb_inst->define_class("MemoryVector2D");
  auto rclass = mrb_inst->define_class("MemoryVector", rclass2D);
  
  rclass2D.define_method("initialize", memvect::init, ARGS_REQ(1));
  rclass.define_method("initialize", memvect::init, ARGS_REQ(1));

  rclass2D.define_method("x", memvect::get_x, ARGS_NONE());
  rclass2D.define_method("y", memvect::get_y, ARGS_NONE());
  rclass.define_method("z", memvect::get_z, ARGS_NONE());
  rclass.define_method("w", memvect::get_w, ARGS_NONE());

  rclass2D.define_method("x=", memvect::set_x, ARGS_REQ(1));
  rclass2D.define_method("y=", memvect::set_y, ARGS_REQ(1));
  rclass.define_method("z=", memvect::set_z, ARGS_REQ(1));
  rclass.define_method("w=", memvect::set_w, ARGS_REQ(1));

  memvect2dclass = rclass2D;
  return rclass;
}

mrb_value ruby::wrap_memory_vector(math::Vector *vect)
{
  static auto rclass = create_memory_vector_class();

  auto ptr = mrb_inst->wrap_native_ptr(vect);
  return rclass.new_inst(ptr);
}

mrb_value ruby::wrap_memory_vector(math::Vector2D *vect)
{
  auto ptr = mrb_inst->wrap_native_ptr(vect);
  return memvect2dclass.new_inst(ptr);
}

#pragma endregion

// ----------------------------------------------------------------------------

#pragma region Vector

// ----------------------------------------------------------------------------

mrb_data_type ruby::mrb_vector_type;

// ----------------------------------------------------------------------------

namespace vect
{
  using namespace ruby;

  static mrb_value vnew(mrb_state *mrb, mrb_value)
  {
    static auto vclass = mrb_class_get(mrb, "Vector");
    
    mrb_float x = 0, y = 0, z = 0, w = 0;
    mrb_get_args(mrb, "|ffff", &x, &y, &z, &w);

    auto vect = new math::Vector{(float)x, (float)y, (float)z, (float)w};
    auto obj = mrb_data_object_alloc(mrb, vclass, vect, &mrb_vector_type);
    return mrb_obj_value(obj);
  }

  static void free(mrb_state *, void *mem)
  {
    auto vect = (math::Vector*) mem;
    delete vect;
  }

  #pragma region Getters

  static mrb_value get_x(mrb_state *mrb, mrb_value self)
  {
    auto vect = (math::Vector *)mrb_data_get_ptr(mrb, self, &mrb_vector_type);
    return mrb_float_value(mrb, vect->x);
  }

  static mrb_value get_y(mrb_state *mrb, mrb_value self)
  {
    auto vect = (math::Vector *)mrb_data_get_ptr(mrb, self, &mrb_vector_type);
    return mrb_float_value(mrb, vect->y);
  }

  static mrb_value get_z(mrb_state *mrb, mrb_value self)
  {
    auto vect = (math::Vector *)mrb_data_get_ptr(mrb, self, &mrb_vector_type);
    return mrb_float_value(mrb, vect->z);
  }

  static mrb_value get_w(mrb_state *mrb, mrb_value self)
  {
    auto vect = (math::Vector *)mrb_data_get_ptr(mrb, self, &mrb_vector_type);
    return mrb_float_value(mrb, vect->w);
  }

  #pragma endregion

  #pragma region Setters

  static mrb_value set_x(mrb_state *mrb, mrb_value self)
  {
    mrb_float value;
    mrb_get_args(mrb, "f", &value);
    auto vect = (math::Vector *)mrb_data_get_ptr(mrb, self, &mrb_vector_type);
    return mrb_float_value(mrb, vect->x = (float)value);
  }

  static mrb_value set_y(mrb_state *mrb, mrb_value self)
  {
    mrb_float value;
    mrb_get_args(mrb, "f", &value);
    auto vect = (math::Vector *)mrb_data_get_ptr(mrb, self, &mrb_vector_type);
    return mrb_float_value(mrb, vect->y = (float)value);
  }

  static mrb_value set_z(mrb_state *mrb, mrb_value self)
  {
    mrb_float value;
    mrb_get_args(mrb, "f", &value);
    auto vect = (math::Vector *)mrb_data_get_ptr(mrb, self, &mrb_vector_type);
    return mrb_float_value(mrb, vect->z = (float)value);
  }

  static mrb_value set_w(mrb_state *mrb, mrb_value self)
  {
    mrb_float value;
    mrb_get_args(mrb, "f", &value);
    auto vect = (math::Vector *)mrb_data_get_ptr(mrb, self, &mrb_vector_type);
    return mrb_float_value(mrb, vect->w = (float)value);
  }

  #pragma endregion


}

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_vector_init(mrb_state *mrb)
{
  ruby::mrb_vector_type.dfree = vect::free;
  ruby::mrb_vector_type.struct_name = "math::Vector";

  auto vclass = mrb_define_class(mrb, "Vector", mrb->object_class);
  
  mrb_define_class_method(mrb, vclass, "new", vect::vnew, ARGS_OPT(4));

  mrb_define_method(mrb, vclass, "x", vect::get_x, ARGS_NONE());
  mrb_define_method(mrb, vclass, "y", vect::get_y, ARGS_NONE());
  mrb_define_method(mrb, vclass, "z", vect::get_z, ARGS_NONE());
  mrb_define_method(mrb, vclass, "w", vect::get_w, ARGS_NONE());
  
  mrb_define_method(mrb, vclass, "x=", vect::set_x, ARGS_REQ(1));
  mrb_define_method(mrb, vclass, "y=", vect::set_y, ARGS_REQ(1));
  mrb_define_method(mrb, vclass, "z=", vect::set_z, ARGS_REQ(1));
  mrb_define_method(mrb, vclass, "w=", vect::set_w, ARGS_REQ(1));
}

#pragma endregion

// ----------------------------------------------------------------------------

mrb_value ruby::create_new_vector(const math::Vector& v)
{
  return mrb_inst->get_class("Vector").functions["new"].call(v.x, v.y, v.z, v.w);
}

// ----------------------------------------------------------------------------



