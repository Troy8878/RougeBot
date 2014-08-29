/*********************************
 * RubyEngine.cpp
 * Connor Hilarides
 * Created 2014/08/12
 *********************************/

#pragma once

#include "Common.h"
#include "RubyInterop.h"
#include "Game.h"

#include "mruby/data.h"
#include "mruby/class.h"
#include "mruby/compile.h"
#include "mruby/string.h"
#include "mruby/error.h"
#include "mruby/array.h"

// ----------------------------------------------------------------------------

using namespace ruby;

ruby_engine *ruby_engine::global_engine;

// ----------------------------------------------------------------------------

ruby_engine::ruby_engine()
  : symbols(*this)
{
  mrb = mrb_open();
}

// ----------------------------------------------------------------------------

ruby_engine::ruby_engine(mrb_state *mrb)
  : mrb(mrb), transient(true), symbols(*this)
{
}

// ----------------------------------------------------------------------------

ruby_engine::~ruby_engine()
{
  if (!transient)
    mrb_close(mrb);
}

// ----------------------------------------------------------------------------

bool ruby_engine::evaluate_asset(const std::string& asset)
{
  auto container = GetGame()->Respack["Scripts"];
  RELEASE_AFTER_SCOPE(container);

  auto resource = container->GetResource(asset);
  RELEASE_AFTER_SCOPE(resource);

  if (!resource)
    return false;

  auto size = resource->Size + 1;
  char *data = new char[size];
  memcpy_s(data, size, resource->Data, resource->Size);
  data[size - 1] = 0;

  mrb_load_string(mrb, data);
  log_and_clear_error();

  return true;
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

ruby_value ruby_engine::wrap_native_ptr(void *ptr)
{
  return ruby_value{mrb_cptr_value(mrb, ptr), this};
}

// ----------------------------------------------------------------------------

void *ruby_engine::unwrap_native_ptr(mrb_value value)
{
  return mrb_cptr(value);
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

ruby_value ruby_func::call_argv(const ruby_value *values, mrb_int num)
{
  mrb_value mvalues[128] = {0};

  if (num > 128)
    throw std::exception("Over 128 params? >.> y u do dis? srsly, tell me");

  for (int i = 0; i < num; ++i)
    mvalues[i] = values[i];

  ruby_value result{mrb_nil_value(), engine};
  result = mrb_funcall_argv(*engine, invokee, funid, num, mvalues);

  engine->log_and_clear_error();

  return result;
}

// ----------------------------------------------------------------------------

template <>
ruby_value ruby_func::call()
{
  return call_argv(nullptr, 0);
}

// ----------------------------------------------------------------------------

void ruby_engine::log_and_clear_error()
{
  if (!mrb->exc)
    return;

  auto prevcolor = console::fg_color();

  std::cerr << console::fg::yellow 
            << "[WARNING] mruby error" << std::endl;
  mrb_print_error(mrb);
  std::cerr << std::endl;

  std::cerr << prevcolor;

  mrb->exc = nullptr;
}

// ----------------------------------------------------------------------------

