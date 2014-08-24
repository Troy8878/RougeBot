/*********************************
 * RubyEngine.cpp
 * Connor Hilarides
 * Created 2014/08/12
 *********************************/

#pragma once

#include "Common.h"
#include "RubyInterop.h"
#include "mruby/data.h"
#include "mruby/class.h"

// ----------------------------------------------------------------------------

using namespace ruby;

static ruby_engine _global_engine;
ruby_engine *ruby_engine::global_engine = &_global_engine;

// ----------------------------------------------------------------------------

ruby_engine::ruby_engine()
  : symbols(*this)
{
  mrb = mrb_open();
}

// ----------------------------------------------------------------------------

ruby_engine::~ruby_engine()
{
  mrb_close(mrb);
}

// ----------------------------------------------------------------------------

ruby_class ruby_engine::define_class(const char *name, RClass *baseClass)
{
  if (!baseClass)
    baseClass = mrb->object_class;

  return ruby_class{this, mrb_define_class(mrb, name, baseClass)};
}

// ----------------------------------------------------------------------------

ruby_class ruby_engine::get_class(const char *name)
{
  return ruby_class{this, mrb_class_get(mrb, name)};
}

// ----------------------------------------------------------------------------

ruby_module ruby_engine::define_module(const char *name)
{
  return ruby_module{this, mrb_define_module(mrb, name)};
}

// ----------------------------------------------------------------------------

ruby_module ruby_engine::get_module(const char *name)
{
  return ruby_module{this, mrb_module_get(mrb, name)};
}

// ----------------------------------------------------------------------------

void ruby_engine::define_const(const char *name, mrb_value value)
{
  mrb_define_global_const(mrb, name, value);
}

// ----------------------------------------------------------------------------

mrb_data_type ruby::mrb_dt_native_ptr = {"NativePtrWrapper"};

// ----------------------------------------------------------------------------

ruby_value ruby_engine::wrap_native_ptr(void *ptr)
{
  static ruby_class native_data_class;
  static bool data_class_init = false;

  if (!data_class_init)
  {
    data_class_init = true;
    native_data_class = define_class("NativeData");
  }
  
  mrb_data_object_alloc(mrb, native_data_class, ptr, &mrb_dt_native_ptr);

  return ruby_value{};
}

// ----------------------------------------------------------------------------

///////////////////////////////////////
// 'Other' ruby helper implementations

ruby_gc_guard::ruby_gc_guard(mrb_state *mrb)
  : mrb(mrb), arena(mrb_gc_arena_save(mrb))
{
}

// ----------------------------------------------------------------------------

ruby_gc_guard::~ruby_gc_guard()
{
  if (mrb)
  {
    mrb_gc_arena_restore(mrb, arena);
    mrb_garbage_collect(mrb);
  }
}

// ----------------------------------------------------------------------------

mrb_sym ruby_symbol_manager::operator[](const char *name) const
{
  return mrb_intern_cstr(engine, name);
};

// ----------------------------------------------------------------------------

ruby_func ruby_function_manager::operator[](mrb_sym funid) const
{
  return ruby_func{&engine, invokee, funid};
}

// ----------------------------------------------------------------------------

ruby_func ruby_function_manager::operator[](const char *name) const
{
  return operator[](mrb_intern_cstr(engine, name));
}

// ----------------------------------------------------------------------------

ruby_value ruby_func::operator()(const ruby_value *values, mrb_int num)
{
  mrb_value mvalues[128];
  if (num > 128)
    throw std::exception("Over 128 params? >.> y u do dis?");

  for (int i = 0; i < num; ++i)
    mvalues[i] = values[i];

  ruby_value result{mrb_nil_value(), engine};
  result = mrb_funcall_argv(*engine, invokee, funid, num, values);
  return result;
}

// ----------------------------------------------------------------------------

