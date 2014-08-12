/*********************************
 * RubyClass.cpp
 * Connor Hilarides
 * Created 2014/08/12
 *********************************/

#pragma once

#include "Common.h"
#include "RubyInterop.h"

namespace ruby
{
  void ruby_class::define_alias(const char *name1, const char *name2)
  {
    mrb_define_alias(*_engine, _class, name1, name2);
  }

  void ruby_class::define_const(const char *name, mrb_value value)
  {
    mrb_define_const(*_engine, _class, name, value);
  }

  void ruby_class::define_method(const char *name, mrb_func_t func, mrb_aspec aspec)
  {
    mrb_define_class_method(*_engine, _class, name, func, aspec);
  }


}
