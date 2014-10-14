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
}

Roguelike game("Game 200 Project", GetModuleHandle(NULL));

extern "C" int IsAvxSupported();

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
  performance::register_guard glperf("The Game");
  
  CreateConsole();

  std::cout << "AVX Support: " << IsAvxSupported() << std::endl;

  HttpUri uri("cloudsdale://connorcpu:derp@cloudsdale.org:80/clouds/hammock?haxor=true#pro");
  HttpClient client;

  game.Run();
  return 0;
}
