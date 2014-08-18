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
  ruby_value::ruby_value(const mrb_value& value, ruby_engine *engine)
    : mrb_value(value), _engine(engine)
  {
  }
}