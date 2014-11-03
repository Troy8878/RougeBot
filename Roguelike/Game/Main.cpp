/*********************************
 * Main.cpp
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#include "Common.h"
#include "PropertyAPI/PropertyServer.h"

static void CreateConsole()
{
  AllocConsole();
  FILE *file;
  freopen_s(&file, "CONOUT$", "wt", stdout);
  freopen_s(&file, "CONOUT$", "wt", stderr);
  freopen_s(&file, "CONIN$", "rt", stdin);

  std::ios::sync_with_stdio();
}

Roguelike game("Roguebot", GetModuleHandle(NULL));

extern "C" int IsAvxSupported();
extern "C" __declspec(noreturn) void GameRunGame()
{
  performance::register_guard glperf("The Game");
  
  #if !PRODUCTION
  CreateConsole();
  #endif

  #if !defined(AVX_BUILD) && PRODUCTION
  if (IsAvxSupported())
  {
    if (fs::exists(fs::path("Game_AVX.exe")))
    {
      CreateProcessW(L"Game_AVX.exe", nullptr,
                     nullptr, nullptr, FALSE,
                     0, nullptr, nullptr,
                     nullptr, nullptr);
      _exit(0);
    }
  }
  #endif

  std::cout << console::fg::white;
  std::cout << "AVX Support: " << IsAvxSupported() << std::endl;

  #if !PRODUCTION
  PropertyServer debug_server;
  #endif

  game.Run();
  _exit(0);
}

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{

  GameRunGame();
}
