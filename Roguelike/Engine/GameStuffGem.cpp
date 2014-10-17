/*********************************
 * GameStuffGem.cpp
 * Connor Hilarides
 * Created 2014/05/30
 *********************************/

#include "Common.h"
#include "Level.h"

#include "mruby.h"

// ----------------------------------------------------------------------------

#pragma region Random stuff

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

  auto arg_count = mrb_get_args(mrb, "|n*", &type, &extra_absorbtion, &absorbed);

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
  else if (type == int_sym)
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
  // WARNING C4702: Unreachable code
  //return mrb_nil_value();
}

// ----------------------------------------------------------------------------

extern "C" mrb_value ruby_message_box(mrb_state *mrb, mrb_value self)
{
  (self);

  char *text, *title;
  mrb_int options;

  auto argc = mrb_get_args(mrb, "zz|i", &title, &text, &options);
  if (argc < 3)
    options = 0;

  mrb_int result = MessageBox(NULL, (text), (title), UINT(options));

  mrb_sym result_sym = mrb_intern_cstr(mrb, "unknown_result");
  switch (result)
  {
    case IDABORT: result_sym = mrb_intern_cstr(mrb, "abort"); break;
    case IDCANCEL: result_sym = mrb_intern_cstr(mrb, "cancel"); break;
    case IDCONTINUE: result_sym = mrb_intern_cstr(mrb, "continue"); break;
    case IDIGNORE: result_sym = mrb_intern_cstr(mrb, "ignore"); break;
    case IDNO: result_sym = mrb_intern_cstr(mrb, "no"); break;
    case IDOK: result_sym = mrb_intern_cstr(mrb, "ok"); break;
    case IDRETRY: result_sym = mrb_intern_cstr(mrb, "retry"); break;
    case IDTRYAGAIN: result_sym = mrb_intern_cstr(mrb, "try_again"); break;
    case IDYES: result_sym = mrb_intern_cstr(mrb, "yes"); break;
  };

  return mrb_symbol_value(result_sym);
}

// ----------------------------------------------------------------------------

extern "C" mrb_value ruby_clearscreen(mrb_state *, mrb_value)
{
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  if (!console)
    return mrb_false_value();

  DWORD cw;
  CONSOLE_SCREEN_BUFFER_INFO info;
  GetConsoleScreenBufferInfo(console, &info);

  auto conSize = info.dwSize.X * info.dwSize.Y;
  FillConsoleOutputCharacter(console, ' ', conSize, COORD{0,0}, &cw);

  GetConsoleScreenBufferInfo(console, &info);
  FillConsoleOutputAttribute(console, info.wAttributes, conSize, COORD{0,0}, &cw);

  SetConsoleCursorPosition(console, COORD{0,0});

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
  auto& dev = *GetGame()->GameDevice;
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

static mrb_value mrb_quit_game(mrb_state *, mrb_value)
{
  GetGame()->Stop();
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_gamestuff_gem_init(mrb_state *mrb)
{
  auto gameClass = mrb_define_class(mrb, "Game", mrb->object_class);
  auto kernel = mrb->kernel_module;

  mrb_define_method(mrb, kernel, "rand", ruby_rand, ARGS_OPT(3));
  mrb_define_method(mrb, kernel, "cls", ruby_clearscreen, ARGS_NONE());

  mrb_define_class_method(mrb, gameClass, "toggle_debug_draw", mrb_toggle_debug_draw, ARGS_NONE());
  mrb_define_class_method(mrb, gameClass, "switch_level", mrb_switch_level, ARGS_REQ(1));
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

}

// ----------------------------------------------------------------------------

