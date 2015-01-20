/*********************************
 * RubyD2D.cpp
 * Connor Hilarides
 * Created 2015/01/13
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"

// ----------------------------------------------------------------------------

mrb_data_type d2d_cxt_dt;
mrb_data_type d2d_token;

// ----------------------------------------------------------------------------

static mrb_value d2d_cxt_instance(mrb_state *mrb, mrb_value klass);

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_d2d_cxt_init(mrb_state *mrb)
{
  auto rclass = mrb_define_class(mrb, "Direct2D", mrb->object_class);
  mrb_define_method(mrb, rclass, "instance", d2d_cxt_instance, MRB_ARGS_NONE());
}

// ----------------------------------------------------------------------------

static mrb_value d2d_cxt_instance(mrb_state *mrb, mrb_value klass)
{
  auto rclass = mrb_class_ptr(klass);
  auto datap = &GetGame()->GameDevice->D2D;
  auto data = mrb_data_object_alloc(mrb, rclass, datap, &d2d_cxt_dt);
  return mrb_obj_value(data);
}

// ----------------------------------------------------------------------------


