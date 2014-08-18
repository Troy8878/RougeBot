/*********************************
 * RubyValue.cpp
 * Connor Hilarides
 * Created 2014/08/17
 *********************************/

#pragma once

#include "Common.h"
#include "RubyInterop.h"

namespace ruby
{
  ruby_module::ruby_module(ruby_engine *engine, RClass *module)
    : ruby_class(engine, module)
  {
  }
}