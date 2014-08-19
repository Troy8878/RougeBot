/*********************************
 * Main.cpp
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#include "Game\Roguelike.h"

#pragma comment(lib, "dxguid.lib")

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

struct uwot
{
  __declspec(property(get = __get_thing, put = __set_thing)) int thing;

  int __get_thing()
  {
    std::random_device rng;
    std::uniform_int_distribution<int> random;

    return random(rng);
  }

  void __set_thing(int value)
  {
    std::cout << "uwot " << value << std::endl;
  }
};

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT)
{
  IFDEBUG(createConsole());

  uwot a;
  std::cout << a.thing << std::endl;
  a.thing = 10;

  Roguelike game("Game 200 Project", hInstance);
  game.run();
}
