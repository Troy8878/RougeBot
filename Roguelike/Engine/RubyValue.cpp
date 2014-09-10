/*********************************
 * RubyValue.cpp
 * Connor Hilarides
 * Created 2014/08/17
 *********************************/

#pragma once

#include "Common.h"
#include "RubyInterop.h"

#include "mruby/value.h"
#include "mruby/string.h"
#include "mruby/data.h"
#include "mruby/class.h"
#include "mruby/array.h"

using namespace ruby;

ruby_value::ruby_value(const mrb_value& value, ruby_engine *engine)
  : mrb_value(value), _engine(engine)
{
}

// ----------------------------------------------------------------------------

ruby_value::~ruby_value()
{
  *this = nullptr;
}

// ----------------------------------------------------------------------------

ruby_value::ruby_value(ruby_value&& moving)
  : ruby_value(static_cast<const ruby_value&>(moving))
{
  moving.mrb_value::operator=(mrb_nil_value());
}

// ----------------------------------------------------------------------------

ruby_class ruby_value::get_class()
{
  return ruby_class{_engine, mrb_class(*_engine, *this)};
}

// ----------------------------------------------------------------------------

ruby_value& ruby_value::operator=(ruby_value&& moving)
{
  operator=(static_cast<const ruby_value&>(moving));
  moving.mrb_value::operator=(mrb_nil_value());

  return *this;
}

// ----------------------------------------------------------------------------

ruby_value& ruby_value::operator=(const mrb_value& value)
{
  return set_mrbv(value);
}

// ----------------------------------------------------------------------------

ruby_value::operator int64_t() 
{
  mrb_value res = functions["to_i"].call();
  return mrb_fixnum(res);
}

// ----------------------------------------------------------------------------

ruby_value::operator int32_t() 
{
  mrb_value res = functions["to_i"].call();
  return (int32_t) mrb_fixnum(res);
}

// ----------------------------------------------------------------------------

ruby_value::operator double() 
{
  mrb_value res = functions["to_f"].call();
  return mrb_float(res);
}

// ----------------------------------------------------------------------------

ruby_value::operator float() 
{
  mrb_value res = functions["to_f"].call();
  return (float) mrb_float(res);
}

// ----------------------------------------------------------------------------

ruby_value& ruby_value::operator=(const char *string)
{
  set_mrbv(mrb_str_new_cstr(*_engine, string));
  return *this;
}

// ----------------------------------------------------------------------------

ruby_value& ruby_value::operator=(const std::string& string)
{
  set_mrbv(mrb_str_new(*_engine, string.c_str(), string.size()));
  return *this;
}

// ----------------------------------------------------------------------------

ruby_value::operator const char *()
{
  if (mrb_string_p(*this))
  {
    return mrb_string_value_cstr(*_engine, this);
  }

  return functions["to_s"].call();
}

// ----------------------------------------------------------------------------

ruby_value::operator std::vector<ruby_value>()
{
  auto len = mrb_ary_len(*_engine, *this);
  
  std::vector<ruby_value> values;
  for (int i = 0; i < len; ++i)
  {
    ruby_value v{mrb_ary_entry(*this, i), _engine};
    values.push_back(v);
  }

  return values;
}

// ----------------------------------------------------------------------------

ruby_value& ruby_value::set_mrbv(const mrb_value& val)
{
  if (_engine)
  {
    _engine->log_and_clear_error();
    mrb_gc_mark_value(*_engine, *this);
  }

  mrb_value::operator=(val);
  return *this;
}

// ----------------------------------------------------------------------------

ruby_value& ruby_value::operator=(const math::Vector& vector)
{
  auto vclass = _engine->get_class("Vector");
  auto vinst = vclass.new_inst(vector.x, vector.y, vector.z, vector.w);

  return set_mrbv(vinst);
}

// ----------------------------------------------------------------------------

ruby_value& ruby_value::operator=(const math::Vector2D& vector)
{
  auto vclass = _engine->get_class("Vector");
  auto vinst = vclass.new_inst(vector.x, vector.y);

  return set_mrbv(vinst);
}

// ----------------------------------------------------------------------------

ruby_value::operator math::Vector()
{
  float x = (float) functions["x"].call().functions["to_f"].call();
  float y = (float) functions["y"].call().functions["to_f"].call();
  float z = (float) functions["z"].call().functions["to_f"].call();
  float w = (float) functions["w"].call().functions["to_f"].call();
  
  return math::Vector{x, y, z, w};
}

// ----------------------------------------------------------------------------

ruby_value::operator math::Vector2D()
{
  float x = (float) functions["x"].call().functions["to_f"].call();
  float y = (float) functions["y"].call().functions["to_f"].call();
  
  return math::Vector2D{x, y};
}

// ----------------------------------------------------------------------------

