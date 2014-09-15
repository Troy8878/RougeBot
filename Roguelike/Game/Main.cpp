/*********************************
 * Main.cpp
 * Connor Hilarides
 * Created 2014/05/28
 *********************************/

#include "Common.h"

static void createConsole()
{
  AllocConsole();
  FILE *file;
  freopen_s(&file, "CONOUT$", "wt", stdout);
  freopen_s(&file, "CONOUT$", "wt", stderr);
  freopen_s(&file, "CONIN$", "rt", stdin);
}

Roguelike game("Game 200 Project", GetModuleHandle(NULL));

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
  createConsole();
  game.Run();
  return 0;
}
