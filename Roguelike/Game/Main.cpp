/*********************************
 * Main.cpp
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#include "Common.h"
#include "Helpers\BucketAllocator.h"

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

static mrb_value rb_test_method(mrb_state *mrb, mrb_value value)
{
  mrb_value num;

  mrb_get_args(mrb, "i", &num);

  std::cout << mrb_fixnum(num) << std::endl;
  return value;
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT)
{
  IFDEBUG(
  {
    createConsole();
  });

  RegisterEngineComponents();

  using namespace ruby;
  auto tclass = ruby_engine::global_engine->define_class("TestClass");
  tclass.define_class_method("asdf", rb_test_method, ARGS_REQ(1));

  auto cval = mrb_obj_value(tclass.mrb_handle());
  mrb_funcall(*ruby_engine::global_engine, cval, "asdf", 1, mrb_fixnum_value(20));

  Roguelike game("Game 200 Project", hInstance);
  game.Run();
}
