/*********************************
 * mrbgem-regexp.cpp
 * Connor Hilarides
 * Created 2014/09/06
 *********************************/

#include "mruby.h"
#include "mruby/data.h"
#include "mruby/array.h"
#include "mruby/string.h"
#include "mruby/variable.h"
#include "mruby/hash.h"
#include "mruby/gems/regexp.h"

#include <regex>

// ----------------------------------------------------------------------------

#define STDREGEX_IV_NAME "stdregex"

mrb_data_type mrb_regex_data;

static mrb_value mrb_regexp_create(mrb_state *mrb, const char *pattern, mrb_int opts);
static void mrb_regexp_free_struct(mrb_state *mrb, void *data);

// ----------------------------------------------------------------------------

static mrb_value mrb_regexp_compile(mrb_state *mrb, mrb_value self);
static mrb_value mrb_regexp_initialize(mrb_state *mrb, mrb_value self);
static mrb_value mrb_regexp_inspect(mrb_state *mrb, mrb_value self);

static mrb_value mrb_regexp_equals(mrb_state *mrb, mrb_value self);
static mrb_value mrb_regexp_whole_match(mrb_state *mrb, mrb_value self);
static mrb_value mrb_regexp_is_match(mrb_state *mrb, mrb_value self);
static mrb_value mrb_regexp_matches(mrb_state *mrb, mrb_value self);
static mrb_value mrb_regexp_casefold(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_regexp_init(mrb_state *mrb)
{
  mrb_regex_data.dfree = mrb_regexp_free_struct;
  mrb_regex_data.struct_name = "std::regex";

  auto regexp = mrb_define_class(mrb, "Regexp", mrb->object_class);

  mrb_define_const(mrb, regexp, "IGNORECASE", mrb_fixnum_value(std::regex::icase));
  mrb_define_const(mrb, regexp, "EXTENDED", mrb_fixnum_value(std::regex::extended));

  mrb_define_class_method(mrb, regexp, "compile", mrb_regexp_compile, MRB_ARGS_ARG(1, 1));

  mrb_define_method(mrb, regexp, "initialize", mrb_regexp_initialize, MRB_ARGS_ARG(1, 1));
  mrb_define_method(mrb, regexp, "inspect", mrb_regexp_inspect, ARGS_NONE());
  
  mrb_define_method(mrb, regexp, "==", mrb_regexp_equals, ARGS_REQ(1));
  mrb_define_method(mrb, regexp, "===", mrb_regexp_whole_match, ARGS_REQ(1));
  mrb_define_method(mrb, regexp, "=~", mrb_regexp_is_match, ARGS_REQ(1));
  
  mrb_define_method(mrb, regexp, "match", mrb_regexp_matches, MRB_ARGS_ARG(1, 1));
  mrb_define_method(mrb, regexp, "casefold?", mrb_regexp_casefold, ARGS_NONE());
  mrb_define_method(mrb, regexp, "eql?", mrb_regexp_equals, ARGS_REQ(1));
}

// ----------------------------------------------------------------------------

extern "C" mrb_value mrb_regexp_split(mrb_state *mrb, mrb_value str_v, mrb_value regexp, mrb_int lim)
{
  auto segments = mrb_ary_new(mrb);
  std::string str = mrb_str_to_cstr(mrb, str_v);

  if (lim == 0)
    lim = std::numeric_limits<mrb_int>::max();
  
  static mrb_sym regex_sym = mrb_intern_lit(mrb, STDREGEX_IV_NAME);
  auto regexp_v = mrb_iv_get(mrb, regexp, regex_sym);
  auto& regex = *(std::regex *) mrb_data_get_ptr(mrb, regexp_v, &mrb_regex_data);

  if (!std::regex_search(str, regex))
  {
    mrb_ary_push(mrb, segments, str_v);
    return segments;
  }
  
  std::smatch matches;
  auto it = str.cbegin();

  while (std::regex_search(it, str.cend(), matches, regex))
  {
    for (auto& match : matches)
    {
      if (match.matched)
      {
        std::string range{it, match.first};
        auto newstr = mrb_str_new_cstr(mrb, range.c_str());
        mrb_ary_push(mrb, segments, newstr);

        it = match.second;

        --lim;
        if (lim <= 0)
          break;
      }
    }
  }

  std::string range{it, str.cend()};
  auto newstr = mrb_str_new_cstr(mrb, range.c_str());
  mrb_ary_push(mrb, segments, newstr);

  return segments;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_regexp_create(mrb_state *mrb, const char *pattern, mrb_int opts)
{
  try
  {
    auto regexp = new std::regex(pattern, (std::regex::flag_type)opts);
    auto data = mrb_data_object_alloc(mrb, mrb->object_class, regexp, &mrb_regex_data);
  
    return mrb_obj_value(data);
  }
  catch (std::regex_error& e)
  {
    mrb_raise(mrb, mrb->eException_class, e.what());
  }
}

// ----------------------------------------------------------------------------

static void mrb_regexp_free_struct(mrb_state *, void *data)
{
  delete reinterpret_cast<std::regex *>(data);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_regexp_compile(mrb_state *mrb, mrb_value self)
{
  auto rclass = mrb_class_get(mrb, "Regexp");
  mrb_value *args;
  mrb_int argc;
  mrb_get_args(mrb, "*", &args, &argc);

  return mrb_obj_new(mrb, rclass, argc, args);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_regexp_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value pattern;
  mrb_value options = mrb_fixnum_value(0);

  mrb_get_args(mrb, "o|o", &pattern, &options);

  // parse string options
  if (!mrb_fixnum_p(options))
  {
    std::string opt_str = mrb_str_to_cstr(mrb, options);
    options = mrb_fixnum_value(0);

    std::regex icase{"i"};
    if (std::regex_search(opt_str, icase))
      mrb_fixnum(options) |= std::regex::icase;

    std::regex extended{"x"};
    if (std::regex_search(opt_str, extended))
      mrb_fixnum(options) |= std::regex::extended;
  }

  static mrb_sym regex_sym = mrb_intern_lit(mrb, STDREGEX_IV_NAME);

  if (mrb_string_p(pattern)) // new regexp
  {
    auto regexp = mrb_regexp_create(mrb, mrb_str_to_cstr(mrb, pattern), mrb_fixnum(options));
    mrb_iv_set(mrb, self, regex_sym, regexp);
  }
  else // copy regexp
  {
    mrb_iv_set(mrb, self, regex_sym, mrb_iv_get(mrb, pattern, regex_sym));
  }

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_regexp_inspect(mrb_state *mrb, mrb_value self)
{
  return mrb_any_to_s(mrb, self);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_regexp_equals(mrb_state *mrb, mrb_value self)
{
  mrb_raise(mrb, mrb->eException_class, "std::regex does not support comparing instances");
}

// ----------------------------------------------------------------------------

static mrb_value mrb_regexp_whole_match(mrb_state *mrb, mrb_value self)
{
  const char *str;
  mrb_get_args(mrb, "z", &str);

  static mrb_sym regex_sym = mrb_intern_lit(mrb, STDREGEX_IV_NAME);

  auto regexp_v = mrb_iv_get(mrb, self, regex_sym);
  auto& regexp = *(std::regex *) mrb_data_get_ptr(mrb, regexp_v, &mrb_regex_data);

  return mrb_bool_value(std::regex_match(str, regexp));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_regexp_is_match(mrb_state *mrb, mrb_value self)
{
  const char *str;
  mrb_get_args(mrb, "z", &str);

  static mrb_sym regex_sym = mrb_intern_lit(mrb, STDREGEX_IV_NAME);

  auto regexp_v = mrb_iv_get(mrb, self, regex_sym);
  auto& regexp = *(std::regex *) mrb_data_get_ptr(mrb, regexp_v, &mrb_regex_data);

  return mrb_bool_value(std::regex_search(str, regexp));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_regexp_matches(mrb_state *mrb, mrb_value self)
{
  mrb_value str_v;
  mrb_int pos = 0;
  mrb_get_args(mrb, "S|i", &str_v, &pos);

  const std::string str = mrb_str_to_cstr(mrb, str_v);
  
  static mrb_sym regex_sym = mrb_intern_lit(mrb, STDREGEX_IV_NAME);
  auto regexp_v = mrb_iv_get(mrb, self, regex_sym);
  auto& regex = *(std::regex *) mrb_data_get_ptr(mrb, regexp_v, &mrb_regex_data);
  
  auto it = str.cbegin() + pos;
  if (!std::regex_search(it, str.cend(), regex))
    return mrb_nil_value();
  
  auto results = mrb_ary_new(mrb);
  
  std::smatch matches;
  while (std::regex_search(it, str.cend(), matches, regex))
  {
    for (auto& match : matches)
    {
      if (match.matched)
      {
        auto newstr = mrb_str_new_cstr(mrb, match.str().c_str());
        mrb_ary_push(mrb, results, newstr);

        it = match.second;
      }
    }
  }

  return results;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_regexp_casefold(mrb_state *mrb, mrb_value self)
{
  static mrb_sym regex_sym = mrb_intern_lit(mrb, STDREGEX_IV_NAME);

  auto regexp_v = mrb_iv_get(mrb, self, regex_sym);
  auto& regexp = *(std::regex *) mrb_data_get_ptr(mrb, regexp_v, &mrb_regex_data);

  return mrb_bool_value((regexp.flags() & std::regex::icase) != 0);
}

// ----------------------------------------------------------------------------

std::regex& mrb_regexp_cppregex(mrb_state *mrb, mrb_value regexp)
{
  static mrb_sym regex_sym = mrb_intern_lit(mrb, STDREGEX_IV_NAME);

  auto regexp_v = mrb_iv_get(mrb, regexp, regex_sym);
  return *(std::regex *) mrb_data_get_ptr(mrb, regexp_v, &mrb_regex_data);
}

// ----------------------------------------------------------------------------
