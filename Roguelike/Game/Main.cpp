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

Roguelike game("Game 200 Project", GetModuleHandle(NULL));

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
  IFDEBUG(createConsole());

  vtprintf("I heard that you like %v %v\n", 500, "Toast slices");
  
  game.Run();
}
