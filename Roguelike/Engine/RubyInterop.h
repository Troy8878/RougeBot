/*********************************
 * RubyInterop.h
 * Connor Hilarides
 * Created 2014/08/12
 *********************************/

#pragma once

#include "mruby.h"
#include "mruby/data.h"
#include "mruby/hash.h"

#include <string>

// I should mention here that I'm using the snake_case naming convention
// because that is the naming convention of Ruby.

namespace ruby
{

// ----------------------------------------------------------------------------

  class ruby_engine;
  class ruby_class;
  class ruby_module;
  class ruby_value;
  class ruby_func;

// ----------------------------------------------------------------------------

  class ruby_engine
  {
  public:
    static ruby_engine *global_engine;

    ruby_engine();
    ~ruby_engine();

    ruby_engine(const ruby_engine&) = delete;
    ruby_engine& operator=(const ruby_engine&) = delete;

    inline mrb_state *mrb_handle() { return mrb; }
    inline operator mrb_state *() { return mrb; }

    ruby_class define_class(const char *className, RClass *baseClass = nullptr);
    ruby_class get_class(const char *className);

    ruby_module define_module(const char *moduleName);
    ruby_module get_module(const char *moduleName);

    void define_const(const char *name, mrb_value value);

    ruby_value wrap_native_ptr(void *ptr);
    void *unwrap_native_ptr(mrb_value value);

    template <typename MapType>
    ruby_value hash_from_map(const MapType& map);

  private:
    mrb_state *mrb;
  };

// ----------------------------------------------------------------------------

  class ruby_class
  {
  protected:
    ruby_engine *_engine;
    RClass *_class;
    
  public:
    ruby_class() = default;
    ruby_class(ruby_engine *engine, RClass *_class);

    inline RClass *mrb_handle() { return _class; }
    inline operator RClass *() { return _class; }

    void define_alias(const char *name1, const char *name2);
    void define_const(const char *name, mrb_value value);
    void define_method(const char *name, mrb_func_t func, mrb_aspec aspec);
    void define_class_method(const char *name, mrb_func_t func, mrb_aspec aspec);
    ruby_class define_class(const char *name, RClass *baseClass = nullptr);

    ruby_value new_inst(ruby_value *values, mrb_int num);

    template <mrb_int count>
    ruby_value new_inst(ruby_value (&values)[count]);
  };

// ----------------------------------------------------------------------------

  class ruby_module : public ruby_class
  {
  public:
    ruby_module(ruby_engine *engine, RClass *module);

    void define_module_method(const char *name, mrb_func_t func, mrb_aspec aspec);
    
  };

// ----------------------------------------------------------------------------

  class ruby_value : public mrb_value
  {
    ruby_engine *_engine;
    typedef mrb_int _mrb_int;
    typedef mrb_float _mrb_float;

  public:
    ruby_value(const mrb_value& value = mrb_nil_value(), ruby_engine *engine = ruby_engine::global_engine);
    ~ruby_value();

    ruby_value& operator=(const mrb_value& value);

    ruby_value& operator=(nullptr_t);

    ruby_value& operator=(mrb_int i);
    ruby_value& operator=(mrb_float f);
    operator _mrb_int();
    operator _mrb_float();

    ruby_value& operator=(const char *string);
    ruby_value& operator=(const std::string& string);
    operator const char *();
    operator std::string();

  private:
    void set_mrbv(const mrb_value& val);
  };

// ----------------------------------------------------------------------------

  class ruby_func
  {
    ruby_engine *_engine;

  public:
  };

// ----------------------------------------------------------------------------

  class ruby_gc_guard
  {
    mrb_state *mrb;
    int arena;

  public:
    ruby_gc_guard(mrb_state *mrb);
    ~ruby_gc_guard();
  };

// ----------------------------------------------------------------------------

  template <mrb_int count>
  ruby_value ruby_class::new_inst(ruby_value (&values)[count])
  {
    return new_inst(values, count);
  }

// ----------------------------------------------------------------------------

  template <typename MapType>
  ruby_value ruby_engine::hash_from_map(const MapType& map)
  {
    auto hash = mrb_hash_new(mrb);

    ruby_value key{mrb_nil_value(), this};
    ruby_value value{mrb_nil_value(), this};

    for (auto& pair : map)
    {
      key = pair.first;
      value = pair.second;

      mrb_hash_set(mrb, hash, key, value);
    }

    return ruby_value{hash, this};
  }

// ----------------------------------------------------------------------------

  extern mrb_data_type mrb_dt_native_ptr;
}


