/*********************************
 * RubyInit.cpp
 * Connor Hilarides
 * Created 2014/05/29
 *********************************/

#include "Common.h"

#include "Helpers\ModuleResource.h"
#include "ModelBuilder.h"

#include "mruby.h"
#include "mruby\compile.h"

extern "C"
{

  void mrb_init_mrblib(mrb_state *mrb)
  {
    ruby::ruby_engine engine{mrb};

    engine.evaluate_asset("mrblib/array.rb");
    engine.evaluate_asset("mrblib/class.rb");
    engine.evaluate_asset("mrblib/compar.rb");
    engine.evaluate_asset("mrblib/enum.rb");
    engine.evaluate_asset("mrblib/error.rb");
    engine.evaluate_asset("mrblib/hash.rb");
    engine.evaluate_asset("mrblib/kernel.rb");
    engine.evaluate_asset("mrblib/numeric.rb");
    engine.evaluate_asset("mrblib/print.rb");
    engine.evaluate_asset("mrblib/range.rb");
    engine.evaluate_asset("mrblib/string.rb");
  }

  // mrbgem protos
  void mrb_mruby_vector_init(mrb_state *mrb);
  void mrb_mruby_fiber_gem_init(mrb_state* mrb);
  void mrb_mruby_gamestuff_gem_init(mrb_state *mrb);
  void mrb_mruby_print_gem_init(mrb_state* mrb);
  void mrb_mruby_math_gem_init(mrb_state* mrb);
  void mrb_mruby_array_ext_gem_init(mrb_state* mrb);
  void mrb_mruby_hash_ext_gem_init(mrb_state *mrb);
  void mrb_mruby_eval_gem_init(mrb_state* mrb);
  void mrb_mruby_regexp_init(mrb_state *mrb);

  void mrb_init_mrbgems(mrb_state *mrb)
  {
    ruby::ruby_engine engine{mrb};

    // Much faster than the vanilla vector class I made :P
    mrb_mruby_vector_init(mrb);
    engine.evaluate_asset("Core/vector.rb");

    // Fiber gem
    mrb_mruby_fiber_gem_init(mrb);

    // Load print gem (puts is nice to have for debugging :P)
    mrb_mruby_print_gem_init(mrb);
    engine.evaluate_asset("gems/mrb-print/print.rb");

    // You know, somebody might like to have math :P
    mrb_mruby_math_gem_init(mrb);

    // Load the enumerator gem
    engine.evaluate_asset("gems/mrb-enumerator/enumerator.rb");

    // I like doing stuff with arrays and hashes :P
    mrb_mruby_array_ext_gem_init(mrb);
    engine.evaluate_asset("gems/mrb-array-ext/array.rb");
    mrb_mruby_hash_ext_gem_init(mrb);
    engine.evaluate_asset("gems/mrb-hash-ext/hash.rb");

    // Eval can be nice, especially if you wanna make a console :3
    mrb_mruby_eval_gem_init(mrb);

    // Regexp is nice as well :P
    mrb_mruby_regexp_init(mrb);

    // Load the gamestuff that I've defined to help out
    mrb_mruby_gamestuff_gem_init(mrb);

    ModelBuilder::InitializeRubyModule(mrb);
  }

  void mrb_final_mrbgems(mrb_state *)
  {
  }
}

