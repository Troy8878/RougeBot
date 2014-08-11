/*********************************
 * RubyInit.cpp
 * Connor Hilarides
 * Created 2014/05/29
 *********************************/

#include "Common.h"

#include "Helpers\ModuleResource.h"

#include "mruby.h"
#include "mruby\compile.h"

#include "Engine\resource.h"

static const int libfiles[] = 
{
  IDR_RBARRAY,
  IDR_RBCLASS,
  IDR_RBCOMPAR,
  IDR_RBENUM,
  IDR_RBERROR,
  IDR_RBHASH,
  IDR_RBKERNEL,
  IDR_RBNUMERIC,
  IDR_RBPRINT,
  IDR_RBRANGE,
  IDR_RBSTRING,
};

inline void loadRubyRes(mrb_state *mrb, int res_id)
{
  ModuleResource res(res_id, "RB");
  auto text_value = res.readAsString();

  mrb_load_string(mrb, text_value.c_str());
}

extern "C"
{

  void mrb_init_mrblib(mrb_state *mrb)
  {
    for (auto res_id : libfiles)
    {
      loadRubyRes(mrb, res_id);
    }
  }

  // mrbgem protos
  void mrb_mruby_fiber_gem_init(mrb_state* mrb);
  void mrb_mruby_gamestuff_gem_init(mrb_state *mrb);

  void mrb_init_mrbgems(mrb_state *mrb)
  {
    mrb_mruby_fiber_gem_init(mrb);
    loadRubyRes(mrb, IDR_RB_GEM_ENUMERATOR);
    mrb_mruby_gamestuff_gem_init(mrb);
  }

  void mrb_final_mrbgems(mrb_state *mrb)
  {
    (mrb);
  }
}

