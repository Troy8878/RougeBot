/*********************************
 * RubyHelp.h
 * Connor Hilarides
 * Created 2014/05/29
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "mruby.h"
#include "mruby\string.h"
#include "mruby\gc.h"
#include "mruby\error.h"

#include <string>
#include <Engine/EntitySection.h>

inline std::string mrb_to_s(mrb_state *mrb, mrb_value value, bool clear_error = true, bool *has_error = nullptr)
{
  if (mrb->exc)
  {
    if (has_error)
      *has_error = true;

    value = mrb_funcall(mrb, mrb_obj_value(mrb->exc), "inspect", 0);
    mrb->exc = nullptr;
  }
  else 
  {
    if (has_error)
      *has_error = false;

    if (!mrb_obj_is_kind_of(mrb, value, mrb->string_class))
      value = mrb_funcall(mrb, value, "inspect", 0);
  }

  auto cstr = mrb_string_value_cstr(mrb, &value);
  return std::string { cstr };
}

inline bool mrb_errord(mrb_state *mrb)
{
  return !!mrb->exc;
}

inline void mrb_get_error(mrb_state *mrb, std::string& message, std::string& backtrace)
{
  if (!mrb_errord(mrb))
    throw basic_exception("mruby did not just raise an error");

  message = mrb_to_s(mrb, mrb_funcall(mrb, mrb_obj_value(mrb->exc), "inspect", 0));
  backtrace = mrb_to_s(mrb, mrb_get_backtrace(mrb));
}


