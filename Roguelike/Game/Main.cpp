/*********************************
 * Main.cpp
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#include "Common.h"
#include "Helpers\BucketAllocator.h"

#include <iomanip>

#include "mruby/error.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "DXGI.lib")

#ifdef _DEBUG
static void createConsole()
{
  AllocConsole();
  FILE *file;
  freopen_s(&file, "CONOUT$", "wt", stdout);
  freopen_s(&file, "CONOUT$", "wt", stderr);
  freopen_s(&file, "CONIN$", "rt", stdin);
}
#endif

static mrb_value rb_print_all_params(mrb_state *mrb, mrb_value self)
{
  ruby::ruby_engine engine{mrb}; // wrap the engine for the ruby_value helper

  mrb_value *argv;
  mrb_int argc;

  // just get the args as an array
  mrb_get_args(mrb, "*", &argv, &argc);

  // print all of the values with to_s
  ruby::ruby_value v{mrb_nil_value(), &engine};
  for (int i = 0; i < argc; ++i)
  {
    v = argv[i];
    v = v.functions["to_s"].call();

    std::cout << static_cast<std::string>(v) << std::endl;
  }

  return self;
}

Roguelike game("Game 200 Project", GetModuleHandle(NULL));

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
  IFDEBUG(createConsole());

  game.Run();
}
