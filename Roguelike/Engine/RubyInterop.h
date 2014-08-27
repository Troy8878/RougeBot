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
  class ruby_symbol_manager;

// ----------------------------------------------------------------------------

  class ruby_symbol_manager
  {
    ruby_engine& engine;

  public:
    ruby_symbol_manager(ruby_engine& engine) 
      : engine(engine) 
    {
    };

    mrb_sym operator[](const char *name) const;

    NO_COPY_CONSTRUCTOR(ruby_symbol_manager);
    NO_ASSIGNMENT_OPERATOR(ruby_symbol_manager);
  };

// ----------------------------------------------------------------------------

  class ruby_function_manager
  {
    ruby_engine& engine;
    mrb_value invokee;

  public:
    ruby_function_manager(ruby_engine& engine, mrb_value invokee) 
      : engine(engine), invokee(invokee) 
    {
    };

    ruby_func operator[](mrb_sym funid) const;
    ruby_func operator[](const char *name) const;

    NO_ASSIGNMENT_OPERATOR(ruby_function_manager);
  };

// ----------------------------------------------------------------------------

  class ruby_engine
  {
  public:
    static ruby_engine *global_engine;

    ruby_engine();
    ruby_engine(mrb_state *mrb);

    ~ruby_engine();

    const ruby_symbol_manager symbols;

    ruby_engine(const ruby_engine&) = delete;
    ruby_engine& operator=(const ruby_engine&) = delete;

    inline mrb_state *mrb_handle() { return mrb; }
    inline operator mrb_state *() { return mrb; }

    bool evaluate_asset(const std::string& asset);

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
    bool transient = false;
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

    ruby_value new_inst_argv(ruby_value *values, mrb_int num);

    template <mrb_int count>
    ruby_value new_inst_argv(ruby_value (&values)[count]);

    template <typename... Args>
    ruby_value new_inst(const Args&... args);

    PROPERTY(get = _GetFuncMgr) ruby_function_manager functions;

  public:
    ruby_function_manager _GetFuncMgr()
    {
      return ruby_function_manager{*_engine, mrb_obj_value(_class)};
    }
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

  public:
    ruby_value(const mrb_value& value = mrb_nil_value(), ruby_engine *engine = ruby_engine::global_engine);
    ~ruby_value();

    ruby_value(ruby_value&& moving);
    ruby_value& operator=(ruby_value&& moving);

    ruby_class get_class();

    ruby_value& operator=(const mrb_value& value);

    ruby_value& operator=(nullptr_t) { return set_mrbv(mrb_nil_value()); }

    ruby_value& operator=(int64_t i) { return set_mrbv(mrb_fixnum_value(i)); }
    ruby_value& operator=(int32_t i) { return set_mrbv(mrb_fixnum_value(i)); }
    ruby_value& operator=(double f) { return set_mrbv(mrb_float_value(*_engine, f)); }
    operator int64_t();
    explicit operator int32_t();
    operator double();
    explicit operator float();

    ruby_value& operator=(const char *string);
    ruby_value& operator=(const std::string& string);
    operator const char *();

    operator std::vector<ruby_value>();

    // Custom class conversions
    ruby_value& operator=(const math::Vector& vector);
    operator math::Vector();

    void silent_reset() { mrb_value::operator=(mrb_nil_value()); }

    PROPERTY(get = _GetFuncMgr) ruby_function_manager functions;

  private:
    ruby_value& set_mrbv(const mrb_value& val);

  public:
    ruby_function_manager _GetFuncMgr()
    {
      return ruby_function_manager{*_engine, *this};
    }
  };

// ----------------------------------------------------------------------------

  class ruby_func
  {
    ruby_engine *engine;
    mrb_value invokee;
    mrb_sym funid;

  public:
    ruby_func(ruby_engine *engine, mrb_value invokee, mrb_sym funid)
      : engine(engine), invokee(invokee), funid(funid)
    {
    }

    ruby_value call_argv(const ruby_value *values, mrb_int num);

    template <typename... Args>
    ruby_value call(const Args&... args);
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
  ruby_value ruby_class::new_inst_argv(ruby_value (&values)[count])
  {
    return new_inst_argv(values, count);
  }

// ----------------------------------------------------------------------------

  template <>
  ruby_value ruby_class::new_inst();

  template <typename... Args>
  ruby_value ruby_class::new_inst(const Args&... args)
  {
    ruby_value values[sizeof...(args)];

    variadic_push_array(values, 0, args...);

    return new_inst_argv(values);
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

  template <> 
  ruby_value ruby_func::call();

  template <typename... Args>
  ruby_value ruby_func::call(const Args&... args)
  {
    ruby_value values[sizeof...(args)];

    variadic_push_array(values, 0, args...);

    return call_argv(values, sizeof...(args));
  }

// ----------------------------------------------------------------------------

  extern mrb_data_type mrb_dt_native_ptr;

// ----------------------------------------------------------------------------
}


