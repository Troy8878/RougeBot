﻿/*********************************
 * GameStuffGem.cpp
 * Connor Hilarides
 * Created 2014/05/30
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "Level.h"
#include <shellapi.h>

#include "mruby.h"

// ----------------------------------------------------------------------------

#pragma region Random stuff
#include <PropertyAPI/PropertyServer.h>
#include "CODA.h"

extern "C" mrb_value ruby_rand(mrb_state *mrb, mrb_value)
{
  static mrb_value *extra_absorbtion;
  static mrb_int absorbed;

  static std::random_device rng;

  static mrb_sym float_sym = mrb_intern_cstr(mrb, "float");
  static mrb_sym int_sym = mrb_intern_cstr(mrb, "int");

  mrb_sym type = float_sym;
  mrb_float fmin = 0;
  mrb_float fmax = 1;
  mrb_int imin = 0;
  mrb_int imax = std::numeric_limits<mrb_int>::max();

  mrb_int arg_count;
  mrb_get_args(mrb, "|n*", &type, &extra_absorbtion, &absorbed);

  if (type == float_sym)
  {
    arg_count = mrb_get_args(mrb, "|nff", &type, &fmin, &fmax);

    if (arg_count == 2)
    {
      fmax = fmin;
      fmin = 0;
    }

    std::uniform_real_distribution<mrb_float> dist(fmin, fmax);
    return mrb_float_value(mrb, dist(rng));
  }
  if (type == int_sym)
  {
    arg_count = mrb_get_args(mrb, "|nii", &type, &imin, &imax);

    if (arg_count == 2)
    {
      imax = imin;
      imin = 0;
    }

    std::uniform_int_distribution<mrb_int> dist(imin, imax);
    return mrb_fixnum_value(dist(rng));
  }

  mrb_raise(mrb, mrb_class_get(mrb, "RuntimeError"), "Type argument must be :int or :float");
  SHUT_UP_RESHARPER_THERE_IS_NO_WAY_THIS_FUNCTION_RETURNS_HERE(mrb_nil_value());
}

// ----------------------------------------------------------------------------

extern "C" mrb_value ruby_message_box(mrb_state *mrb, mrb_value)
{
  char *text, *title;
  mrb_int options;

  auto argc = mrb_get_args(mrb, "zz|i", &title, &text, &options);
  if (argc < 3)
    options = 0;

  mrb_int result = MessageBox(nullptr, (text), (title), UINT(options));

  mrb_sym result_sym = mrb_intern_cstr(mrb, "unknown_result");
  switch (result)
  {
  case IDABORT: result_sym = mrb_intern_lit(mrb, "abort");
    break;
  case IDCANCEL: result_sym = mrb_intern_lit(mrb, "cancel");
    break;
  case IDCONTINUE: result_sym = mrb_intern_lit(mrb, "continue");
    break;
  case IDIGNORE: result_sym = mrb_intern_lit(mrb, "ignore");
    break;
  case IDNO: result_sym = mrb_intern_lit(mrb, "no");
    break;
  case IDOK: result_sym = mrb_intern_lit(mrb, "ok");
    break;
  case IDRETRY: result_sym = mrb_intern_lit(mrb, "retry");
    break;
  case IDTRYAGAIN: result_sym = mrb_intern_lit(mrb, "try_again");
    break;
  case IDYES: result_sym = mrb_intern_lit(mrb, "yes");
    break;
  }

  return mrb_symbol_value(result_sym);
}

// ----------------------------------------------------------------------------

extern "C" mrb_value ruby_clearscreen(mrb_state *, mrb_value)
{
  for (size_t i = 0; i < 50; ++i)
    std::cout << std::endl;

  return mrb_nil_value();
}

#pragma endregion

// ----------------------------------------------------------------------------

static mrb_value mrb_level_root_entity(mrb_state *, mrb_value)
{
  auto level = GetGame()->CurrentLevel;
  if (!level)
    return mrb_nil_value();

  return level->RootEntity->RubyWrapper;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_toggle_debug_draw(mrb_state *, mrb_value)
{
  auto &dev = *GetGame()->GameDevice;
  dev.DebugDraw = !dev.DebugDraw;

  return mrb_bool_value(dev.DebugDraw);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_switch_level(mrb_state *mrb, mrb_value)
{
  const char *levelName;
  mrb_get_args(mrb, "z", &levelName);

  GetGame()->ChangeLevel(levelName);

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_reload_level(mrb_state *, mrb_value)
{
  GetGame()->RestartLevel();
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_quit_game(mrb_state *, mrb_value)
{
  GetGame()->Stop();
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_browse(mrb_state *, mrb_value)
{
  if (!propviewer)
  {
    propviewer = new PropertyServer;
  }

  ShellExecuteA(nullptr, "open", 
                "http://localhost:5430/game/index.html",
                nullptr, nullptr, SW_SHOWNORMAL);
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_confirm_destructive_action(mrb_state *mrb, mrb_value)
{
  mrb_value text;
  mrb_value callback = mrb_nil_value();
  mrb_get_args(mrb, "S|&", &text, &callback);

  static ConfirmationOfDestructiveAction action;
  auto answer = action.Confirm(mrb_str_to_stdstring(text), "No", "Yes");
  auto mrbanswer = mrb_bool_value(answer);

  if (!mrb_nil_p(callback) && answer)
    mrb_yield(mrb, callback, mrb_nil_value());

  return mrbanswer;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_json_parse(mrb_state *mrb, mrb_value)
{
  ruby::ruby_engine engine{mrb};
  mrb_value mrb_str;
  mrb_get_args(mrb, "S", &mrb_str);

  return engine.json_to_value(json::value::parse(mrb_str_to_stdstring(mrb_str)));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_json_stringify(mrb_state *mrb, mrb_value)
{
  ruby::ruby_engine engine{mrb};
  mrb_value obj;
  mrb_get_args(mrb, "o", &obj);

  try
  {
    auto json = engine.value_to_json(obj);
    auto str = json.pretty_print();
    return mrb_str_new(mrb, str.c_str(), str.size());
  }
  catch (basic_exception &e)
  {
    mrb_raise(mrb, E_RUNTIME_ERROR, e.what());
  }
}

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_gamestuff_gem_init(mrb_state *mrb)
{
  auto gameClass = mrb_define_class(mrb, "Game", mrb->object_class);
  auto kernel = mrb->kernel_module;

  mrb_define_method(mrb, kernel, "rand", ruby_rand, ARGS_OPT(3));
  mrb_define_method(mrb, kernel, "cls", ruby_clearscreen, ARGS_NONE());
  mrb_define_method(mrb, kernel, "browse", mrb_browse, ARGS_NONE());
  mrb_define_method(mrb, kernel, "confirm_destructive_action!", mrb_confirm_destructive_action, ARGS_ANY());
  mrb_define_method(mrb, kernel, "coda!", mrb_confirm_destructive_action, ARGS_ANY());

  mrb_define_class_method(mrb, gameClass, "toggle_debug_draw", mrb_toggle_debug_draw, ARGS_NONE());
  mrb_define_class_method(mrb, gameClass, "switch_level", mrb_switch_level, ARGS_REQ(1));
  mrb_define_class_method(mrb, gameClass, "reload_level", mrb_reload_level, ARGS_NONE());
  mrb_define_class_method(mrb, gameClass, "quit!", mrb_quit_game, ARGS_NONE());

#pragma region MessageBox stuff

  mrb_define_class_method(mrb, gameClass, "message_box", ruby_message_box, MRB_ARGS_ARG(2, 1));

  auto mbModule = mrb_define_module(mrb, "MessageBox");
  mrb_define_const(mrb, mbModule, "TOPMOST", mrb_fixnum_value(0x40000L));

  auto iconModule = mrb_define_module_under(mrb, mbModule, "Icon");
  mrb_define_const(mrb, iconModule, "EXCLAMATION", mrb_fixnum_value(0x30L));
  mrb_define_const(mrb, iconModule, "WARNING", mrb_fixnum_value(0x30L));
  mrb_define_const(mrb, iconModule, "INFORMATION", mrb_fixnum_value(0x40L));
  mrb_define_const(mrb, iconModule, "ASTERISK", mrb_fixnum_value(0x40L));
  mrb_define_const(mrb, iconModule, "QUESTION", mrb_fixnum_value(0x20L));
  mrb_define_const(mrb, iconModule, "STOP", mrb_fixnum_value(0x10L));
  mrb_define_const(mrb, iconModule, "ERROR", mrb_fixnum_value(0x10L));
  mrb_define_const(mrb, iconModule, "HAND", mrb_fixnum_value(0x10L));

  auto sModule = mrb_define_module_under(mrb, mbModule, "s");
  mrb_define_const(mrb, sModule, "HELP", mrb_fixnum_value(0x4000L));
  mrb_define_const(mrb, sModule, "ABORTRETRYIGNORE", mrb_fixnum_value(0x2L));
  mrb_define_const(mrb, sModule, "CANCELTRYCONTINUE", mrb_fixnum_value(0x6L));
  mrb_define_const(mrb, sModule, "OK", mrb_fixnum_value(0x0L));
  mrb_define_const(mrb, sModule, "OKCANCEL", mrb_fixnum_value(0x1L));
  mrb_define_const(mrb, sModule, "RETRYCANCEL", mrb_fixnum_value(0x5L));
  mrb_define_const(mrb, sModule, "YESNO", mrb_fixnum_value(0x4L));
  mrb_define_const(mrb, sModule, "YESNOCANCEL", mrb_fixnum_value(0x3L));

  auto modalModule = mrb_define_module_under(mrb, mbModule, "Modal");
  mrb_define_const(mrb, modalModule, "APPLICATION", mrb_fixnum_value(0x0L));
  mrb_define_const(mrb, modalModule, "SYSTEM", mrb_fixnum_value(0x1000L));
  mrb_define_const(mrb, modalModule, "TASK", mrb_fixnum_value(0x2000L));

#pragma endregion

#pragma region Level Wrapper

  auto levelWrapper = mrb_define_class(mrb, "GameLevelWrapper", mrb->object_class);
  mrb_define_method(mrb, levelWrapper, "root_entity", mrb_level_root_entity, ARGS_NONE());

#pragma endregion

#pragma region JSON
  auto json = mrb_define_module(mrb, "JSON");

  mrb_define_module_function(mrb, json, "parse", mrb_json_parse, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, json, "stringify", mrb_json_stringify, MRB_ARGS_REQ(1));

#pragma endregion

}

// ----------------------------------------------------------------------------