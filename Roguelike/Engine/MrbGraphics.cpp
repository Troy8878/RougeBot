/*********************************
 * MrbGraphics.cpp
 * Connor Hilarides
 * Created 2014/11/19
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"

// ----------------------------------------------------------------------------

static mrb_value mrb_graphics_vsync(mrb_state *mrb, mrb_value);
static mrb_value mrb_graphics_vsync_s(mrb_state *mrb, mrb_value);

static mrb_value mrb_graphics_fullscreen(mrb_state *mrb, mrb_value);
static mrb_value mrb_graphics_fullscreen_s(mrb_state *mrb, mrb_value);

// ----------------------------------------------------------------------------

EXTERN_C void mrb_mruby_graphics_init(mrb_state *mrb)
{
  auto graphics = mrb_define_class(mrb, "Graphics", mrb->object_class);

  mrb_define_class_method(mrb, graphics, "vsync", mrb_graphics_vsync, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, graphics, "vsync=", mrb_graphics_vsync_s, MRB_ARGS_REQ(1));

  mrb_define_class_method(mrb, graphics, "fullscreen", mrb_graphics_fullscreen, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, graphics, "fullscreen=", mrb_graphics_fullscreen_s, MRB_ARGS_REQ(1));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_graphics_vsync(mrb_state *, mrb_value)
{
  return mrb_bool_value(GetGame()->initSettings.vsync);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_graphics_vsync_s(mrb_state *mrb, mrb_value)
{
  mrb_bool value;
  mrb_get_args(mrb, "b", &value);

  GetGame()->initSettings.vsync = !!value;

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_graphics_fullscreen(mrb_state *, mrb_value)
{
  auto *windowDev = static_cast<WindowDevice *>(GetGame()->GameDevice);
  return mrb_bool_value(windowDev->GetFullscreen());
}

// ----------------------------------------------------------------------------

static mrb_value mrb_graphics_fullscreen_s(mrb_state *mrb, mrb_value)
{
  mrb_bool value;
  mrb_get_args(mrb, "b", &value);

  auto *windowDev = static_cast<WindowDevice *>(GetGame()->GameDevice);
  windowDev->SetFullscreen(!!value);

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------
