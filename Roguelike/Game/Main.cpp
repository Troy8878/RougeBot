/*********************************
 * Main.cpp
 * Connor Hilarides
 * Created 2014/05/28
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include "GameConsole.h"
#include "PropertyAPI/PropertyServer.h"

static void CreateConsole()
{
  //AllocConsole();
  //FILE *file;
  //freopen_s(&file, "CONOUT$", "wt", stdout);
  //freopen_s(&file, "CONOUT$", "wt", stderr);
  //freopen_s(&file, "CONIN$", "rt", stdin);

  //consoleStreamBuf.extra = std::cout.rdbuf();
  std::cout.rdbuf(&consoleStreamBuf);
  std::cerr.rdbuf(&consoleStreamBuf);

  //std::ios::sync_with_stdio();
}

extern "C" int IsAvxSupported();
extern "C" __declspec(noreturn) void GameRunGame()
{
  performance::register_guard glperf("The Game");

  Roguelike game("Roguebot", GetModuleHandle(nullptr));

  #if !defined(AVX_BUILD) && PRODUCTION
  if (IsAvxSupported())
  {
    STARTUPINFO info = {sizeof(info)};
    PROCESS_INFORMATION procinfo;

    CreateProcess(nullptr, "Game_AVX.exe", nullptr, nullptr,
                  TRUE, 0, nullptr, nullptr, &info, &procinfo);

    _exit(0);
  }
  #endif
  
  #if !PRODUCTION
  CreateConsole();
  #endif

  std::cout << console::fg::white;
  std::cout << "AVX Support: " << IsAvxSupported() << std::endl;

  game.Run();
  _exit(0);
}

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
  GameRunGame();
}
