/*********************************
 * Main.cpp
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#include "Common.h"
#include "PropertyAPI/PropertyServer.h"

// Change this to 1 for a production build
#define PRODUCTION 0

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
extern "C" void GameRunGame()
{
  performance::register_guard glperf("The Game");
  
  #if !PRODUCTION
  CreateConsole();
  #endif

  std::cout << console::fg::white;
  std::cout << "AVX Support: " << IsAvxSupported() << std::endl;

  #if !PRODUCTION
  PropertyServer debug_server;
  #endif

  game.Run();

  Sleep(100);
}

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
  GameRunGame();
  return 0;
}
