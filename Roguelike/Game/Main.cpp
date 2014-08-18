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

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT)
{
  IFDEBUG(createConsole());
  IFDEBUG(std::cout << sizeof(std::chrono::system_clock::time_point));

  Roguelike game("Game 200 Project", hInstance);
  game.run();
}
