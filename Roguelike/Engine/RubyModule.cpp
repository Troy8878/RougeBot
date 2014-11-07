/*********************************
 * RubyValue.cpp
 * Connor Hilarides
 * Created 2014/08/17
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
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

ruby_module ruby_module::get_module(const char *name)
{
  return ruby_module{_engine, mrb_define_module_under(*_engine, _class, name)};
}

// ----------------------------------------------------------------------------

void ruby_module::define_module_method(const char *name, mrb_func_t func, mrb_aspec aspec)
{
  mrb_define_module_function(*_engine, _class, name, func, aspec);
}

// ----------------------------------------------------------------------------
