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

static bool IsWindowsAtLeast(DWORD major, DWORD minor, WORD spMajor, WORD spMinor)
{
    DWORDLONG condition = 0;
    OSVERSIONINFOEX info = {sizeof(info)};
    info.dwMajorVersion = major;
    info.dwMinorVersion = minor;
    info.wServicePackMajor = spMajor;
    info.wServicePackMinor = spMinor;

    VER_SET_CONDITION(condition, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(condition, VER_MINORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(condition, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
    VER_SET_CONDITION(condition, VER_SERVICEPACKMINOR, VER_GREATER_EQUAL);

    return !!VerifyVersionInfoA(&info, VER_MAJORVERSION | VER_MINORVERSION |
                                VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR, condition);
}

extern "C" int IsAvxSupported();
extern "C" __declspec(noreturn) void GameRunGame()
{
  SetProcessDPIAware();

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
  if (!IsWindowsAtLeast(6, 1, 1, 0))
  {
    MessageBoxA(nullptr, "Please install Windows 7 (Service Pack 1) or better",
                "Missing Requirement", MB_ICONERROR);
    return;
  }

  GameRunGame();
}
