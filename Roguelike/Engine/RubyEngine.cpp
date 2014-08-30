/*********************************
 * RubyEngine.cpp
 * Connor Hilarides
 * Created 2014/08/12
 *********************************/

#pragma once

#include "Common.h"
#include "RubyInterop.h"
#include "Game.h"

#include "mruby/string.h"
#include "mruby/data.h"
#include "mruby/class.h"
#include "mruby/compile.h"
#include "mruby/error.h"
#include "mruby/array.h"
#include "mruby/compile.h"

#include <sstream>

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

  if (!resource)
    return false;
  
  mrbc_context *cxt = mrbc_context_new(mrb);

#ifdef _DEBUG
  cxt->capture_errors = true;

  size_t fname_size = asset.length() + 1;
  cxt->filename = new char[fname_size];
  strcpy_s(cxt->filename, fname_size, asset.c_str());
#endif

  mrb_load_nstring_cxt(mrb, (char *) resource->Data, (int) resource->Size, cxt);

  resource->Release();

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



mrb_value ruby_engine::wrap_native_ptr(void *ptr)
{
  return mrb_cptr_value(mrb, ptr);
}

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

  std::cerr << console::fg::red;

  // Get the backtrace because the error ruins it
  mrb_value b = mrb_exc_backtrace(mrb, mrb_obj_value(mrb->exc));

  // Print the error
  mrb_value s = mrb_funcall(mrb, mrb_obj_value(mrb->exc), "inspect", 0);
  if (mrb_string_p(s)) 
  {
    fwrite(RSTRING_PTR(s), RSTRING_LEN(s), 1, stderr);
    putc('\n', stderr);
  }
  
  mrb_gc_mark_value(mrb, mrb_obj_value(mrb->exc));
  mrb->exc = nullptr;

  // Now print the backtrace
  ruby_value btrace{b, this};
  std::vector<ruby_value> btrace_lines = btrace;

  std::cerr << "Backtrace:" << std::endl;
  size_t i = 0;

#ifdef _DEBUG
  std::ostringstream btb;
#endif

  for (auto& line : btrace_lines)
  {
    std::cerr << "  [" << ++i << "] " 
              << static_cast<std::string>(line) << std::endl;
#ifdef _DEBUG
    btb << "  [" << i << "] " 
        << static_cast<std::string>(line) << std::endl;
#endif

    mrb_gc_mark_value(mrb, line);
  }

#ifdef _DEBUG
  std::string error_message = std::string(RSTRING_PTR(s), RSTRING_PTR(s) + RSTRING_LEN(s));
  std::string backtrace = btb.str();

  __debugbreak();
#endif

  mrb_gc_mark_value(mrb, s);
  mrb_gc_mark_value(mrb, b);

  mrb_full_gc(mrb);
}

// ----------------------------------------------------------------------------

