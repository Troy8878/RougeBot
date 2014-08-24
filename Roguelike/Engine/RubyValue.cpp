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

using namespace ruby;

ruby_value::ruby_value(const mrb_value& value, ruby_engine *engine)
  : mrb_value(value), _engine(engine)
{
}

ruby_value::~ruby_value()
{
  *this = nullptr;
}

ruby_value& ruby_value::operator=(const mrb_value& value)
{
  set_mrbv(value);
  return *this;
}

ruby_value& ruby_value::operator=(nullptr_t)
{
  set_mrbv(mrb_nil_value());
  return *this;
}

ruby_value& ruby_value::operator=(mrb_int i)
{
  set_mrbv(mrb_fixnum_value(i));
  return *this;
}

ruby_value& ruby_value::operator=(mrb_float f)
{
  set_mrbv(mrb_float_value(*_engine, f));
  return *this;
}

ruby_value::operator _mrb_int()
{
  return mrb_fixnum(*this);
}

ruby_value::operator _mrb_float()
{
  return mrb_float(*this);
}

ruby_value& ruby_value::operator=(const char *string)
{
  set_mrbv(mrb_str_new_cstr(*_engine, string));
  return *this;
}

ruby_value& ruby_value::operator=(const std::string& string)
{
  set_mrbv(mrb_str_new(*_engine, string.c_str(), string.size()));
  return *this;
}

ruby_value::operator const char *()
{
  return mrb_string_value_cstr(*_engine, this);
}

ruby_value::operator std::string()
{
  return std::string(static_cast<const char *>(*this));
}

void ruby_value::set_mrbv(const mrb_value& val)
{
  mrb_gc_mark_value(*_engine, *this);
  mrb_value::operator=(val);
}

