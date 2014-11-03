/*********************************
 * RubyInterop.h
 * Connor Hilarides
 * Created 2014/08/12
 *********************************/

#pragma once

#include "mruby.h"
#include "mruby/data.h"
#include "mruby/hash.h"
#include "mruby/value.h"
#include "mruby/variable.h"
#include "mruby/string.h"
#include "mruby/array.h"

#include "Common.h"

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

    mrb_value wrap_native_ptr(void *ptr);
    void *unwrap_native_ptr(mrb_value value);

    template <typename MapType>
    ruby_value hash_from_map(const MapType& map);

    json::value value_to_json(mrb_value value);
    json::value hash_to_json(mrb_value hash);
    json::value array_to_json(mrb_value ary);

    mrb_value json_to_value(json::value jv);
    mrb_value json_to_hash(json::value jobj);
    mrb_value json_to_array(json::value jary);

    void log_and_clear_error();

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

    ruby_class get_class(const char *name);

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

    ruby_module get_module(const char *name);

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

    // Json conversion
    ruby_value& operator=(json::value jv) { return set_mrbv(_engine->json_to_value(jv)); }

    // Custom class conversions
    ruby_value& operator=(const math::Vector& vector);
    ruby_value& operator=(const math::Vector2D& vector);
    operator math::Vector();
    operator math::Vector2D();

    mrb_value silent_reset() 
    { 
      mrb_value c = *this; 
      mrb_value::operator=(mrb_nil_value()); 
      return c;
    }

    PROPERTY(get = _GetFuncMgr) ruby_function_manager functions;

    inline bool is_nil() { return mrb_nil_p(*this); }

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

# define MRB_NATIVE_PTR_SAVED_SYM_NAME "___native_pointer___"

  template <typename T>
  void save_native_ptr(mrb_state *mrb, mrb_value self, T *ptr)
  {
    static mrb_sym ptr_sym = mrb_intern_cstr(mrb, MRB_NATIVE_PTR_SAVED_SYM_NAME);
    mrb_iv_set(mrb, self, ptr_sym, mrb_cptr_value(mrb, (void *) ptr));
  }

// ----------------------------------------------------------------------------
  
  template <typename T>
  T *read_native_ptr(mrb_state *mrb, mrb_value self)
  {
    static mrb_sym ptr_sym = mrb_intern_lit(mrb, MRB_NATIVE_PTR_SAVED_SYM_NAME);
    return reinterpret_cast<T *>(mrb_cptr(mrb_iv_get(mrb, self, ptr_sym)));
  }

// ----------------------------------------------------------------------------

  template <typename T>
  void data_scalar_delete(mrb_state *, void *data)
  {
    delete reinterpret_cast<T *>(data);
  }

  static void data_nop_delete(mrb_state *, void *)
  {
  }

// ----------------------------------------------------------------------------

  template <typename T>
  T *data_get(mrb_state *mrb, mrb_value value)
  {
    const char *correctName = typeid(T).name();

    if (value.tt != MRB_TT_DATA)
    {
      auto badName = mrb_class_name(mrb, mrb_obj_class(mrb, value));
      mrb_raisef(mrb, E_TYPE_ERROR, "wrong argument type %S (expected %S)",
                 mrb_str_new_cstr(mrb, badName), 
                 mrb_str_new_cstr(mrb, correctName));
    }

    RData *data = (RData *) value.value.p;
    const char *realName = data->type->struct_name;

    if (strcmp(realName, correctName) != 0)
    {
      mrb_raisef(mrb, E_TYPE_ERROR, "wrong argument type %S (expected %S)",
                 mrb_str_new_cstr(mrb, realName), 
                 mrb_str_new_cstr(mrb, correctName));
    }

    return (T *) data->data;
  }

// ----------------------------------------------------------------------------

  template <typename T, const mrb_data_type *DT, typename R, R(T::*MFP)(void)>
  mrb_value data_getter_access_integer(mrb_state *mrb, mrb_value self)
  {
    // Sorry, I couldn't resist trying them out (the emoji)
    auto& obj = *(T *) mrb_data_get_ptr(mrb, self, DT);
    return mrb_fixnum_value((mrb_int) (obj.*MFP)());
  }

// ----------------------------------------------------------------------------

  template <typename T, const mrb_data_type *DT, typename R, R(T::*MFP)(void)>
  mrb_value data_getter_access_string(mrb_state *mrb, mrb_value self)
  {
    // Sorry, I couldn't resist trying them out (the emoji)
    auto obj = (T *) mrb_data_get_ptr(mrb, self, DT);
    if (obj == nullptr)
      return mrb_str_new(mrb, "", 0);

    std::string val = static_cast<std::string>((obj->*MFP)());
    return mrb_str_new(mrb, val.c_str(), val.size());
  }

// ----------------------------------------------------------------------------

  inline mrb_int enumerable_length(mrb_state *mrb, mrb_value enumerable)
  {
    static mrb_sym length = mrb_intern_lit(mrb, "length");
    return mrb_fixnum(mrb_funcall_argv(mrb, enumerable, length, 0, nullptr));
  }

// ----------------------------------------------------------------------------

  inline mrb_value enumerable_first(mrb_state *mrb, mrb_value enumerable)
  {
    static mrb_sym first = mrb_intern_lit(mrb, "first");
    return mrb_funcall_argv(mrb, enumerable, first, 0, nullptr);
  }

// ----------------------------------------------------------------------------

  inline mrb_value enumerable_at(mrb_state *mrb, mrb_value enumerable, mrb_int index)
  {
    static mrb_sym subscript = mrb_intern_lit(mrb, "[]");
    static mrb_sym enum_at = mrb_intern_lit(mrb, "enum_at");

    if (mrb_array_p(enumerable))
    {
      return mrb_ary_entry(enumerable, index);
    }
    
    const mrb_value index_val = mrb_fixnum_value(index);
    if (mrb_respond_to(mrb, enumerable, subscript))
    {
      return mrb_funcall_argv(mrb, enumerable, subscript, 1, &index_val);
    }

    if (mrb_respond_to(mrb, enumerable, enum_at))
    {
      return mrb_funcall_argv(mrb, enumerable, enum_at, 1, &index_val);
    }

    mrb_raisef(mrb, mrb->eException_class, "%S is not an Enumerable object", enumerable);
  }

// ----------------------------------------------------------------------------

  struct mrb_ary_iter
  {
    mrb_ary_iter(mrb_state *mrb, mrb_value ary)
      : mrb(mrb), ary(ary)
    {
    }

    mrb_state *mrb;
    mrb_value ary;

    struct iterator
    {
      iterator(mrb_value ary, mrb_int pos)
        : ary(ary), pos(pos)
      {
      }

      bool operator==(const iterator& rhs) const
      {
        return pos == rhs.pos;
      }

      bool operator!=(const iterator& rhs) const
      {
        return pos != rhs.pos;
      }

      mrb_value operator*() const
      {
        return mrb_ary_entry(ary, pos);
      }

      iterator& operator++()
      {
        ++pos;
        return *this;
      }

      iterator& operator--()
      {
        --pos;
        return *this;
      }

      mrb_value ary;
      mrb_int pos;
    };

    iterator begin()
    {
      return iterator(ary, 0);
    }

    iterator end()
    {
      return iterator(ary, mrb_ary_len(mrb, ary));
    }

    iterator rbegin()
    {
      return iterator(ary, mrb_ary_len(mrb, ary) - 1);
    }

    iterator rend()
    {
      return iterator(ary, -1);
    }
  };

  inline mrb_ary_iter array_each(mrb_state *mrb, mrb_value ary)
  {
    return mrb_ary_iter(mrb, ary);
  }

// ----------------------------------------------------------------------------

  extern mrb_data_type mrb_dt_native_ptr;

// ----------------------------------------------------------------------------

}

// ----------------------------------------------------------------------------

mrb_value mrb_nop(mrb_state *, mrb_value);

// ----------------------------------------------------------------------------

inline std::string mrb_str_to_stdstring(mrb_value str)
{
  if (mrb_nil_p(str))
    return std::string{};

  return std::string{RSTRING_PTR(str), RSTRING_PTR(str) + RSTRING_LEN(str)};
}

// ----------------------------------------------------------------------------

extern bool mrb_debug_mbox;

// ----------------------------------------------------------------------------

#define MRB_DECL_SYM(mrb, var, sname) \
  static ::mrb_sym var = mrb_intern_lit(mrb, sname); \
  static ::mrb_value var##_v = mrb_symbol_value(var)

// ----------------------------------------------------------------------------


