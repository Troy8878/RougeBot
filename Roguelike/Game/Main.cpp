/*********************************
 * Main.cpp
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#include "Common.h"
#include "Helpers\BucketAllocator.h"

#include <iomanip>

#include "mruby/error.h"

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
  
  std::cout << "sizeof(math::Vector): " << sizeof(math::Vector) << std::endl;
  std::cout << "sizeof(RData):        " << sizeof(RData) << std::endl;

  game.Run();
  return 0;
}
