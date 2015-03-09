/*
** print.c - Kernel.#p
**
** See Copyright Notice in mruby.h
*/

#include "mruby.h"
#include "mruby/string.h"
#include "mruby/variable.h"

#pragma warning(disable : 4244) // http://msdn.microsoft.com/en-us/library/th7a07tz.aspx

void gwrite(const void *str, size_t size, size_t ct);
void gputc(int c);

static void
printstr(mrb_state *mrb, mrb_value obj)
{
#ifdef ENABLE_STDIO
  char *s;
  int len;

  if (mrb_string_p(obj)) {
    s = RSTRING_PTR(obj);
    len = RSTRING_LEN(obj);
    gwrite(s, len, 1);
  }
#endif
}

void
mrb_p(mrb_state *mrb, mrb_value obj)
{
#ifdef ENABLE_STDIO
  obj = mrb_funcall(mrb, obj, "inspect", 0);
  printstr(mrb, obj);
  gputc('\n');
#endif
}

void
mrb_print_error(mrb_state *mrb)
{
#ifdef ENABLE_STDIO
  mrb_value s;
  
  mrb_print_backtrace(mrb);
  s = mrb_funcall(mrb, mrb_obj_value(mrb->exc), "inspect", 0);
  if (mrb_string_p(s)) {
    gwrite(RSTRING_PTR(s), RSTRING_LEN(s), 1);
    gputc('\n');
  }
#endif
}

void
mrb_show_version(mrb_state *mrb)
{
  mrb_value msg;

  msg = mrb_const_get(mrb, mrb_obj_value(mrb->object_class), mrb_intern_lit(mrb, "MRUBY_DESCRIPTION"));
  printstr(mrb, msg);
  gputc('\n');
}

void
mrb_show_copyright(mrb_state *mrb)
{
  mrb_value msg;

  msg = mrb_const_get(mrb, mrb_obj_value(mrb->object_class), mrb_intern_lit(mrb, "MRUBY_COPYRIGHT"));
  printstr(mrb, msg);
  gputc('\n');
}
