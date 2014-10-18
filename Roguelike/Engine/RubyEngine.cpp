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
  auto prevfg = console::fg_color();
  std::cout << console::fg::blue 
            << "Loading ruby script " << asset 
            << std::endl << prevfg;

  auto container = GetGame()->Respack["Scripts"];
  RELEASE_AFTER_SCOPE(container);

  auto resource = container->GetResource(asset);

  if (!resource)
    return false;
  
  mrbc_context *cxt = mrbc_context_new(mrb);

  cxt->capture_errors = true;

  size_t fname_size = asset.length() + 1;
  cxt->filename = new char[fname_size];
  strcpy_s(cxt->filename, fname_size, asset.c_str());

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
    throw basic_exception("Over 128 params? >.> y u do dis? srsly, tell me");

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

  ruby_gc_guard{mrb};

  auto pfg = console::fg_color();
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

  std::ostringstream btb;
  btb << "Backtrace:" << std::endl;

  for (auto& line : btrace_lines)
  {
    std::cerr << "  [" << ++i << "] " 
              << static_cast<std::string>(line) << std::endl;
    btb << "  [" << i << "] " 
        << static_cast<std::string>(line) << std::endl;

    mrb_gc_mark_value(mrb, line);
  }

  if (mrb_debug_mbox)
  {
    std::string error_message = std::string(RSTRING_PTR(s), RSTRING_PTR(s) + RSTRING_LEN(s));
    std::string backtrace = btb.str();
    std::string full_message = error_message + "\n" + backtrace + "\n\nContinue running?";

    int result = MessageBox(NULL, full_message.c_str(), "Ruby Error", MB_ICONERROR | MB_YESNO);

    if (result == IDNO)
      exit(1);
  }

  std::cerr << pfg;
}

// ----------------------------------------------------------------------------

json::value ruby_engine::value_to_json(mrb_value value)
{
  if (mrb_hash_p(value))
  {
    return hash_to_json(value);
  }
  else if (mrb_array_p(value))
  {
    return array_to_json(value);
  }
  else if (mrb_string_p(value))
  {
    mrb_value value_s = mrb_funcall(mrb, value, "to_s", 0);
    return json::value::string(mrb_str_to_stdstring(value_s));
  }
  else if (mrb_fixnum_p(value))
  {
    return json::value::number((json::value::number_t) mrb_fixnum(value));
  }
  else if (mrb_float_p(value))
  {
    return json::value::number(mrb_float(value));
  }
  else if (mrb_nil_p(value))
  {
    return json::value::null();
  }
  else if (mrb_type(value) == MRB_TT_FALSE || mrb_type(value) == MRB_TT_TRUE)
  {
    return json::value::boolean(!!mrb_bool(value));
  }

  throw basic_exception("Unknown type in hash. "
                       "Please only use basics "
                       "(hash, array, string, num, bool).");
}

// ----------------------------------------------------------------------------

json::value ruby_engine::hash_to_json(mrb_value hash)
{
  json::value jobj = json::value::object();
  auto& object = jobj.as_object();

  auto keys = mrb_hash_keys(mrb, hash);

  auto key_ct = mrb_ary_len(mrb, keys);
  for (mrb_int i = 0; i < key_ct; ++i)
  {
    mrb_value key = mrb_ary_entry(keys, i);
    mrb_value value = mrb_hash_get(mrb, hash, key);
    
    mrb_value key_to_s = mrb_funcall(mrb, key, "to_s", 0);
    std::string key_s = mrb_str_to_stdstring(key_to_s);

    object[key_s] = value_to_json(value);
  }

  return jobj;
}

// ----------------------------------------------------------------------------

json::value ruby_engine::array_to_json(mrb_value ary)
{
  json::value jary = json::value::array();
  auto& array = jary.as_array();

  auto value_ct = mrb_ary_len(mrb, ary);
  for (mrb_int i = 0; i < value_ct; ++i)
  {
    mrb_value value = mrb_ary_entry(ary, i);
    array.push_back(value_to_json(value));
  }

  return jary;
}

// ----------------------------------------------------------------------------

mrb_value ruby_engine::json_to_value(json::value jv)
{
  using namespace json;
  switch (jv.type())
  {
    case json_type::jbool:
      return mrb_bool_value(jv.as_bool());

    case json_type::jnumber:
    {
      auto num = (mrb_float) jv.as_number();
      if (std::fmod(num, 1) == 0)
        return mrb_fixnum_value((mrb_int) num);

      return mrb_float_value(mrb, num);
    }

    case json_type::jobject:
      return json_to_hash(jv);

    case json_type::jarray:
      return json_to_array(jv);

    case json_type::jstring:
    {
      auto& str = jv.as_string();
      return mrb_str_new(mrb, &*str.begin(), str.size());
    }

    default:
      return mrb_nil_value();
  }
}

// ----------------------------------------------------------------------------

mrb_value ruby_engine::json_to_hash(json::value jobj)
{
  mrb_value hash = mrb_hash_new(mrb);

  for (auto& pair : jobj.as_object())
  {
    mrb_value key = mrb_str_new(mrb,&* pair.first.begin(), pair.first.size());
    mrb_value value = json_to_value(pair.second);

    mrb_hash_set(mrb, hash, key, value);
  }

  return hash;
}

// ----------------------------------------------------------------------------

mrb_value ruby_engine::json_to_array(json::value jary)
{
  mrb_value ary = mrb_ary_new(mrb);

  for (auto& jv : jary.as_array())
  {
    mrb_value value = json_to_value(jv);
    mrb_ary_push(mrb, ary, value);
  }

  return ary;
}

// ----------------------------------------------------------------------------

mrb_value mrb_nop(mrb_state *, mrb_value)
{
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

bool mrb_debug_mbox = true;

// ----------------------------------------------------------------------------

