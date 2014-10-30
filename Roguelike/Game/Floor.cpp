/*********************************
* Floor.cpp
* Connor Hilarides
* Created 2014/09/08
*********************************/

#include "Common.h"
#include "Floor.h"
#include "mruby/array.h"

// 1 = WALL

static std::random_device RNG;

static mrb_value mrb_floor_generate(mrb_state *mrb, mrb_value)
{
  mrb_value options;
  mrb_get_args(mrb, "H", &options);
  
  return RoomGenerator().Generate(mrb, options);
}

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_floor_init(mrb_state *mrb)
{
  auto module = mrb_define_module(mrb, "Floor");

  mrb_define_module_function(mrb, module, "generate", mrb_floor_generate, ARGS_OPT(1));
}
