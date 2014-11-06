/*********************************
 * EventTypes.h
 * Connor Hilarides
 * Created 2014/08/12
 *********************************/

#include "Common.h"
#include "EventTypes.h"

#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/value.h"
#include "mruby/variable.h"
#include "mruby/string.h"

// ----------------------------------------------------------------------------


static mrb_value rb_gametime_dt(mrb_state *mrb, mrb_value)
{
  return mrb_float_value(mrb, GetGame()->Time.Dt);
}

// ----------------------------------------------------------------------------

static mrb_value rb_update_event_class()
{
  mrb_state *mrb = *mrb_inst;

  auto gt = mrb_define_class(mrb, "GameTime", mrb->object_class);

  mrb_define_class_method(mrb, gt, "dt", rb_gametime_dt, ARGS_NONE());

  return mrb_obj_value(gt);
}

// ----------------------------------------------------------------------------

mrb_value Events::UpdateEvent::GetRubyWrapper()
{
  static auto wrapper = rb_update_event_class();
  return wrapper;
}

// ----------------------------------------------------------------------------