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

static GameTime& rb_update_event_get_gametime(mrb_state *mrb, mrb_value self)
{
  ruby::ruby_engine engine{mrb};

  static auto gt_sym = mrb_intern_cstr(mrb, "gt_sym");
  auto gt_val = mrb_iv_get(mrb, self, gt_sym);

  return *reinterpret_cast<GameTime *>(engine.unwrap_native_ptr(gt_val));
}

static mrb_value rb_update_event_dt(mrb_state *mrb, mrb_value self)
{
  return mrb_float_value(mrb, rb_update_event_get_gametime(mrb, self).Dt);
}

static mrb_value rb_update_event_init(mrb_state *mrb, mrb_value self)
{
  mrb_value gt_val;
  mrb_get_args(mrb, "o", &gt_val);
  
  static auto gt_sym = mrb_intern_cstr(mrb, "gt_sym");
  mrb_iv_set(mrb, self, gt_sym, gt_val);

  return mrb_nil_value();
}

static mrb_value rb_update_event_class(GameTime *gt)
{
  auto& mrb = *ruby::ruby_engine::global_engine;
  auto rclass = mrb.define_class("UpdateEvent");

  rclass.define_method("initialize", rb_update_event_init, ARGS_REQ(1));
  rclass.define_method("dt", rb_update_event_dt, ARGS_NONE());

  return rclass.new_inst(mrb.wrap_native_ptr(gt)).silent_reset();
}

mrb_value Events::UpdateEvent::GetRubyWrapper()
{
  auto wrapper = rb_update_event_class(&gameTime);
  return wrapper;
}

// ----------------------------------------------------------------------------
