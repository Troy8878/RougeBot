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

  static mrb_value dup(mrb_state *mrb, mrb_value self)
  {
    auto vclass = mrb_class_get(mrb, "Vector");
    static mrb_sym vnew = mrb_intern_cstr(mrb, "new");
    return mrb_funcall_argv(mrb, mrb_obj_value(vclass), vnew, 1, &self);
  }
}

static ruby::ruby_class memvect2dclass;

static ruby::ruby_class create_memory_vector_class(mrb_state *mrb)
{
  ruby::ruby_engine mrb_inst{mrb};

  auto rclass2D = mrb_inst.define_class("MemoryVector2D");
  auto rclass = mrb_inst.define_class("MemoryVector", rclass2D);
  
  rclass2D.define_method("initialize", memvect::init, ARGS_REQ(1));
  rclass.define_method("initialize", memvect::init, ARGS_REQ(1));
  rclass.define_method("dup", memvect::dup, ARGS_NONE());

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
  static auto rclass = mrb_inst->get_class("MemoryVector");

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

  #pragma region New vector

  static mrb_value vnew(mrb_state *mrb, mrb_value)
  {
    static auto vclass = mrb_class_get(mrb, "Vector");

    mrb_value first = mrb_nil_value(), *rest;
    mrb_int crest;
    mrb_get_args(mrb, "|o*", &first, &rest, &crest);
    
    mrb_float x = 0, y = 0, z = 0, w = 0;
    if (mrb_obj_class(mrb, first) == vclass)
    {
      auto oldv = (math::Vector *) 
        mrb_data_get_ptr(mrb, first, &mrb_vector_type);

      x = oldv->x;
      y = oldv->y;
      z = oldv->z;
      w = oldv->w;
    }
    else if (mrb_obj_class(mrb, first) == mrb_class_get(mrb, "MemoryVector"))
    {
      static auto ptr_sym = mrb_intern_cstr(mrb, "vect_ptr");
      auto oldv = (math::Vector *) mrb_cptr(mrb_iv_get(mrb, first, ptr_sym));

      x = oldv->x;
      y = oldv->y;
      z = oldv->z;
      w = oldv->w;
    }
    else
    {
      mrb_get_args(mrb, "|ffff", &x, &y, &z, &w);
    }

    auto vect = new math::Vector{(float)x, (float)y, (float)z, (float)w};
    auto obj = mrb_data_object_alloc(mrb, vclass, vect, &mrb_vector_type);
    return mrb_obj_value(obj);
  }

  static mrb_value scalar(mrb_state *mrb, mrb_value)
  {
    static auto vclass = mrb_class_get(mrb, "Vector");

    mrb_float s;
    mrb_get_args(mrb, "f", &s);

    auto vect = new math::Vector{(float) s, (float) s, (float) s, (float) s};
    auto obj = mrb_data_object_alloc(mrb, vclass, vect, &mrb_vector_type);
    return mrb_obj_value(obj);
  }

  static void free(mrb_state *, void *mem)
  {
    auto vect = (math::Vector*) mem;
    delete vect;
  }

  #pragma endregion

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

  #pragma region Basic Operators

  static mrb_value op_add(mrb_state *mrb, mrb_value self)
  {
    mrb_value oval;
    mrb_get_args(mrb, "o", &oval);

    auto& ov = *(math::Vector *)mrb_data_get_ptr(mrb, oval, &mrb_vector_type);
    auto& sv = *(math::Vector *)mrb_data_get_ptr(mrb, self, &mrb_vector_type);

    sv = sv + ov;

    return self;
  }
  
  static mrb_value op_sub(mrb_state *mrb, mrb_value self)
  {
    mrb_value oval;
    mrb_get_args(mrb, "o", &oval);

    auto& ov = *(math::Vector *)mrb_data_get_ptr(mrb, oval, &mrb_vector_type);
    auto& sv = *(math::Vector *)mrb_data_get_ptr(mrb, self, &mrb_vector_type);

    sv = sv - ov;

    return self;
  }

  static mrb_value op_mul(mrb_state *mrb, mrb_value self)
  {
    mrb_value oval;
    mrb_get_args(mrb, "o", &oval);

    if (mrb_fixnum_p(oval))
      oval = mrb_float_value(mrb, (mrb_float) mrb_fixnum(oval));
    
    auto& sv = *(math::Vector *)mrb_data_get_ptr(mrb, self, &mrb_vector_type);
    if (mrb_float_p(oval))
    {
      sv = sv * (float)mrb_float(oval);
    }
    else
    {
      auto& ov = *(math::Vector *)mrb_data_get_ptr(mrb, oval, &mrb_vector_type);

      sv = sv * ov;
    }

    return self;
  }
  
  static mrb_value op_div(mrb_state *mrb, mrb_value self)
  {
    mrb_value oval;
    mrb_get_args(mrb, "o", &oval);

    if (mrb_fixnum_p(oval))
      oval = mrb_float_value(mrb, (mrb_float) mrb_fixnum(oval));
    
    auto& sv = *(math::Vector *)mrb_data_get_ptr(mrb, self, &mrb_vector_type);
    if (mrb_float_p(oval))
    {
      sv = sv / (float)mrb_float(oval);
    }
    else
    {
      auto& ov = *(math::Vector *)mrb_data_get_ptr(mrb, oval, &mrb_vector_type);

      sv = sv / ov;
    }

    return self;
  }

  #pragma endregion

  #pragma region Complex vector ops

  using namespace DirectX;

  mrb_value length2(mrb_state *mrb, mrb_value self)
  {
    auto& vector = get_ruby_vector(self);
    auto lensq = XMVector3LengthSq(vector);

    return mrb_float_value(mrb, XMVectorGetX(lensq));
  }

  mrb_value length(mrb_state *mrb, mrb_value self)
  {
    auto& vector = get_ruby_vector(self);
    auto len = XMVector3Length(vector);
    
    return mrb_float_value(mrb, XMVectorGetX(len));
  }

  mrb_value normalize(mrb_state *, mrb_value self)
  {
    auto& vector = get_ruby_vector(self);
    vector = XMVector3Normalize(vector);

    return self;
  }

  mrb_value normalized(mrb_state *, mrb_value self)
  {
    auto& vector = get_ruby_vector(self);

    return create_new_vector(XMVector3Normalize(vector));
  }

  #pragma endregion

  #pragma region Equality

  static mrb_value op_eql(mrb_state *mrb, mrb_value self)
  {
    mrb_value other;
    mrb_get_args(mrb, "o", &other);

    XMVECTOR v1 = get_ruby_vector(self);
    XMVECTOR v2 = get_ruby_vector(other);

    return mrb_bool_value(XMVector4Equal(v1, v2));
  }

  static mrb_value op_near(mrb_state *mrb, mrb_value self)
  {
    mrb_value other;
    mrb_float epsilon = 0.01;
    mrb_get_args(mrb, "o|f", &other, &epsilon);

    XMVECTOR v1 = get_ruby_vector(self);
    XMVECTOR v2 = get_ruby_vector(other);

    const float epsilon_f = (float) epsilon;
    const XMVECTOR epsilon_v = XMVectorSet(epsilon_f, epsilon_f, epsilon_f, epsilon_f);

    return mrb_bool_value(XMVector4NearEqual(v1, v2, epsilon_v));
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
  mrb_define_class_method(mrb, vclass, "scalar", vect::scalar, ARGS_REQ(1));
  mrb_define_method(mrb, vclass, "dup", memvect::dup, ARGS_NONE());

  mrb_define_method(mrb, vclass, "x", vect::get_x, ARGS_NONE());
  mrb_define_method(mrb, vclass, "y", vect::get_y, ARGS_NONE());
  mrb_define_method(mrb, vclass, "z", vect::get_z, ARGS_NONE());
  mrb_define_method(mrb, vclass, "w", vect::get_w, ARGS_NONE());
  
  mrb_define_method(mrb, vclass, "x=", vect::set_x, ARGS_REQ(1));
  mrb_define_method(mrb, vclass, "y=", vect::set_y, ARGS_REQ(1));
  mrb_define_method(mrb, vclass, "z=", vect::set_z, ARGS_REQ(1));
  mrb_define_method(mrb, vclass, "w=", vect::set_w, ARGS_REQ(1));

  mrb_define_method(mrb, vclass, "add", vect::op_add, ARGS_REQ(1));
  mrb_define_method(mrb, vclass, "sub", vect::op_sub, ARGS_REQ(1));
  mrb_define_method(mrb, vclass, "mul", vect::op_mul, ARGS_REQ(1));
  mrb_define_method(mrb, vclass, "div", vect::op_div, ARGS_REQ(1));

  mrb_define_method(mrb, vclass, "length2", vect::length2, ARGS_NONE());
  mrb_define_method(mrb, vclass, "length", vect::length, ARGS_NONE());
  mrb_define_method(mrb, vclass, "normalize!", vect::normalize, ARGS_NONE());
  mrb_define_method(mrb, vclass, "normalized", vect::normalized, ARGS_NONE());

  mrb_define_method(mrb, vclass, "==", vect::op_near, ARGS_REQ(1) | ARGS_OPT(1));
  mrb_define_method(mrb, vclass, "===", vect::op_eql, ARGS_REQ(1));
  mrb_define_method(mrb, vclass, "eql?", vect::op_eql, ARGS_REQ(1));
  mrb_define_method(mrb, vclass, "near?", vect::op_near, ARGS_REQ(1) | ARGS_OPT(1));

  create_memory_vector_class(mrb);
}

#pragma endregion

// ----------------------------------------------------------------------------

mrb_value ruby::create_new_vector(const math::Vector& v)
{
  return mrb_inst->get_class("Vector").functions["new"].call(v.x, v.y, v.z, v.w);
}

// ----------------------------------------------------------------------------

math::Vector& ruby::get_ruby_vector(mrb_value value)
{
  auto ptr = mrb_data_get_ptr(*mrb_inst, value, &mrb_vector_type);
  return *(math::Vector *) ptr;
}

// ----------------------------------------------------------------------------



