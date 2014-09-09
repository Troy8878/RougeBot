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

// ----------------------------------------------------------------------------



