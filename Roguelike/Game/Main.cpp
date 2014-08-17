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

template <typename T>
void inferenceTest(std::string& str, void(T::*p)())
{
  UNREFERENCED_PARAMETER(p);
  str = typeid(T).name();
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT)
{
  IFDEBUG(createConsole());

  std::string t;
  inferenceTest(t, &std::string::clear);

  Roguelike game("Game 200 Project", hInstance);
  game.run();
}
