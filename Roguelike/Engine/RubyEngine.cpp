/*********************************
 * RubyEngine.cpp
 * Connor Hilarides
 * Created 2014/08/12
 *********************************/

#pragma once

#include "Common.h"
#include "RubyInterop.h"

namespace ruby
{
  ruby_engine::ruby_engine()
  {
    mrb = mrb_open();
  }

  ruby_engine::~ruby_engine()
  {
    mrb_close(mrb);
  }

  ruby_class ruby_engine::define_class(const char *name, RClass *baseClass)
  {
    if (!baseClass)
      baseClass = mrb->object_class;

    return ruby_class{this, mrb_define_class(mrb, name, baseClass)};
  }

  ruby_class ruby_engine::get_class(const char *name)
  {
    return ruby_class{this, mrb_class_get(mrb, name)};
  }

  ruby_module ruby_engine::define_module(const char *name)
  {
    return ruby_module{this, mrb_define_module(mrb, name)};
  }

  ruby_module ruby_engine::get_module(const char *name)
  {
    return ruby_module{this, mrb_module_get(mrb, name)};
  }

  void ruby_engine::define_const(const char *name, mrb_value value)
  {
    mrb_define_global_const(mrb, name, value);
  }

}

