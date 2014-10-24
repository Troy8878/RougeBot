/*********************************
 * Main.cpp
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#include "Common.h"
#include "Engine/HttpClient.h"

static void CreateConsole()
{
  AllocConsole();
  FILE *file;
  freopen_s(&file, "CONOUT$", "wt", stdout);
  freopen_s(&file, "CONOUT$", "wt", stderr);
  freopen_s(&file, "CONIN$", "rt", stdin);

  std::ios::sync_with_stdio();
}

Roguelike game("Game 200 Project", GetModuleHandle(NULL));

extern "C" int IsAvxSupported();

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
  performance::register_guard glperf("The Game");
  
  CreateConsole();

  std::cout << console::fg::white;
  std::cout << "AVX Support: " << IsAvxSupported() << std::endl;

  game.Run();

  // I don't even know, but this stops it from crashing at exit
  Sleep(100);
  return 0;
}
