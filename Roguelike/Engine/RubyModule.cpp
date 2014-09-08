/*********************************
 * RubyValue.cpp
 * Connor Hilarides
 * Created 2014/08/17
 *********************************/

#pragma once

#include "Common.h"
#include "RubyInterop.h"

using namespace ruby;

// ----------------------------------------------------------------------------

ruby_module::ruby_module(ruby_engine *engine, RClass *module)
  : ruby_class(engine, module)
{
}

// ----------------------------------------------------------------------------

void ruby_module::define_module_method(const char *name, mrb_func_t func, mrb_aspec aspec)
{
  mrb_define_module_function(*_engine, _class, name, func, aspec);
}

// ----------------------------------------------------------------------------
