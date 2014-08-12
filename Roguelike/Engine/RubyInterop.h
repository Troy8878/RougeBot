/*********************************
 * RubyInterop.h
 * Connor Hilarides
 * Created 2014/08/12
 *********************************/

#pragma once

#include "mruby.h"
#include <string>

// I should mention here that I'm using the snake_case naming convention
// because that is the naming convention of Ruby.

namespace ruby
{
  class ruby_engine;
  class ruby_class;
  class ruby_module;
  class ruby_value;

  class ruby_engine
  {
  public:
    ruby_engine();
    ~ruby_engine();

    ruby_engine(const ruby_engine&) = delete;
    ruby_engine& operator=(const ruby_engine&) = delete;

    inline operator mrb_state *() { return mrb; }

    ruby_class define_class(const char *className, RClass *baseClass = nullptr);
    ruby_class get_class(const char *className);

    ruby_module define_module(const char *moduleName);
    ruby_module get_module(const char *moduleName);

    void define_const(const char *name, mrb_value value);

  private:
    mrb_state *mrb;
  };

  class ruby_class
  {
    ruby_engine *_engine;
    RClass *_class;
    
  public:
    ruby_class(ruby_engine *engine, RClass *_class);

    inline RClass *mrb_handle() { return _class; }
    inline operator RClass *() { return _class; }

    void define_alias(const char *name1, const char *name2);
    void define_const(const char *name, mrb_value value);
    void define_method(const char *name, mrb_func_t func, mrb_aspec aspec);
    void define_class(const char *name, mrb_aspec);

    
  };

  class ruby_module : public ruby_class
  {
    ruby_engine *_engine;
    RClass *_module;

  public:
    ruby_module(ruby_engine *engine, RClass *module);
  };

  class ruby_value : mrb_value
  {
    ruby_engine *_engine;

  public:
    ruby_value(ruby_engine *engine);
  };
}


